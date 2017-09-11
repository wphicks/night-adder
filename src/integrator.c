#include <stdlib.h>

#include "vbase.h"
#include "vatomic.h"
#include "vqueue.h"

#include "nathread.h"
#include "natime.h"
#include "vector.h"
#include "particle.h"
#include "particle_pair.h"
#include "integrator.h"

void finalize_frame(Integrator * integ);

void init_Integrator(
    Integrator * integ, int particle_count, Particle * particles) {

  int i;
  int j;
  int k;

  integ->particle_count = particle_count; 
  integ->particles = particles;
  integ->particle_update_count = malloc(integ->particle_count * sizeof(int32_t));


  integ->pair_count = pair_index(
    particle_count - 1, particle_count, particle_count
  );
  integ->queue_buffer = malloc(vqueue_get_bytes_required(integ->pair_count));
  integ->collide_queue = vqueue_create(integ->queue_buffer, integ->pair_count);
  integ->pairs = malloc(integ->pair_count * sizeof(ParticlePair));

  k = 0;
  for (i=0; i < integ->particle_count; ++i) {
    for (j=i+1; j < integ->particle_count; ++j) {
      init_ParticlePair(
        integ->pairs + k, i, j, integ->particle_count,
        integ->particles
      );
      ++k;
    }
    *(integ->particle_update_count + i) = integ->particle_count - 1;
  }

  integ->frame_update_count = integ->particle_count;

  integ->dt = 0.001;
  integ->remaining_time.as_double = 0.0;
  integ->interp_alpha.as_double = 0.0;

  integ->finished_workers = -1;
}

void cleanup_Integrator(Integrator * old_int) {
  free(old_int->pairs);
  free(old_int->queue_buffer);
  free(old_int->particle_update_count);
}

double pair_dist_square(Integrator * integ, ParticlePair * pair) {
  return distance_square(
    &(integ->particles[pair->i].position), &(integ->particles[pair->j].position)
  );
}

void fill_queue(Integrator *integ) {
  int i;
  int j;
  int k;
  k = 0;
  for (i=0; i < integ->particle_count; ++i) {
    for (j=i+1; j < integ->particle_count; ++j) {
      vqueue_push(integ->collide_queue, (void *) (integ->pairs + k));
      ++k;
    }
  }
}

void finalize_frame(Integrator *integ) {
  atomic_double remaining_time;
  atomic_double new_remaining_time;
  vatomic_barrier();
  if (
      vatomic32_compare_exchange(
        &(integ->frame_update_count), 0, integ->particle_count
      ) == 0) {  /* Every particle's position updated once */
    for (;;) { /* Decrement remaining time*/
      remaining_time.as_double = integ->remaining_time.as_double;
      new_remaining_time.as_double = remaining_time.as_double - integ->dt;
      if (
          vatomic64_compare_exchange(
            &(integ->remaining_time.as_int), remaining_time.as_int,
            new_remaining_time.as_int
          ) == remaining_time.as_int) {
        break;
      }
    }
    vatomic_barrier();
    if (integ->remaining_time.as_double < integ->dt) {  /* Frame complete */
      integ->interp_alpha.as_double = integ->remaining_time.as_double / integ->dt;
    } else {  /* Keep calculating, frame not done */
      fill_queue(integ);
    }
  }
}

void finalize_particle(Integrator * integ, int index) {
  vatomic_barrier();
  if (
      vatomic32_compare_exchange(
        integ->particle_update_count + index, 0, integ->particle_count - 1
      ) == 0) {  /* All collisions checked for this particle once */
    /* The following need not be atomic, since particle's position will not be
     * touched by any other thread until after the frame_update_count is
     * decremented to 0. This cannot happen until the following line at the
     * earliest.*/
    update_Particle_position(integ->particles + index, integ->dt);
    vatomic32_decrement(&(integ->frame_update_count));
  }
}

void worker_loop(void * args) {
  Integrator * integ = (Integrator *) args;
  /* TODO: Sweep and prune (Also think about data locality).*/
  void * retrieved;
  ParticlePair * cur_pair;

  for (;;) {
    if (integ->finished_workers >= 0) { /* Work done; clean up */
      vatomic32_increment(&integ->finished_workers);
      return;
    }
    if (vqueue_pop(integ->collide_queue, &retrieved)) {
      cur_pair = (ParticlePair *) retrieved;
      /* Check pair from queue for collisions, updating velocities of each if
       * necessary */
      collide(integ, cur_pair);
      /* Decrement counter for remaining collisions to be checked for each
       * particle */
      vatomic32_decrement(integ->particle_update_count + cur_pair->i);
      vatomic32_decrement(integ->particle_update_count + cur_pair->j);
      /* If all collisions have been checked, particle's velocity has been
       * finalized for this cycle. Go ahead and update its position */
      finalize_particle(integ, cur_pair->i);
      finalize_particle(integ, cur_pair->j);
      /* If all particles have been updated, check to see if we have integrated
       * requested interval of time. If not, refill the queue of pairs to check
       * for collision */
      finalize_frame(integ);
    } /* TODO: Else improve sorting? */
  }

}


void stop_workers(Integrator * integ, int32_t worker_count) {
  integ->finished_workers = 0;
  for (;;) {
    vatomic_barrier();
    if (integ->finished_workers >= worker_count) {
      break;
    }
  }
  integ->finished_workers = -1;
}


void integrate_interval(Integrator * integ, double interval) {
  atomic_double remaining_time;
  atomic_double new_remaining_time;
  for (;;) { /* Decrement remaining time*/
    remaining_time.as_double = integ->remaining_time.as_double;
    new_remaining_time.as_double = remaining_time.as_double + interval;
    if (
        vatomic64_compare_exchange(
          &(integ->remaining_time.as_int), remaining_time.as_int,
          new_remaining_time.as_int
        ) == remaining_time.as_int) {
      break;
    }
  }
  /* If we were already done with the previous interval... */
  if (remaining_time.as_double < integ->dt) {
    fill_queue(integ);
  }
}


int collide(Integrator * integ, ParticlePair * pair) {
  Particle * part1 = integ->particles + pair->i;
  Particle * part2 = integ->particles + pair->j;
  double collision_projection;
  double impulse_coefficient;
  double dist2 = pair_dist_square(integ, pair);
  Vec swap0;
  Vec swap1;
  if (dist2 > pair->square_sum_radii) {
    return 0;
  }

  diff_Vec(&(part2->position), &(part1->position), &swap0); /* Position diff */
  norm_Vec(&swap0, &swap0); /* Collision normal */
  diff_Vec(&(part2->velocity), &(part1->velocity), &swap1); /* Velocity diff */
  collision_projection = dot_Vec(&swap0, &swap1);

  if (collision_projection > 0) { /* Already moving apart */
    return 1;
  }

  impulse_coefficient = (
    (1 + pair->restitution) * collision_projection * pair->reduced_mass
  );

  multiply_Vec(&swap0, impulse_coefficient, &swap0);
  multiply_Vec(&swap0, -part2->inv_mass, &swap1);
  multiply_Vec(&swap0, part1->inv_mass, &swap0);

  atomic_isum_Vec(&(part1->velocity), &swap0);
  atomic_isum_Vec(&(part2->velocity), &swap1);
  return 1;
}
