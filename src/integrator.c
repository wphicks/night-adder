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
      vqueue_push(integ->collide_queue, (void *) (integ->pairs + k));
      ++k;
    }
    *(integ->particle_update_count + i) = 0;
  }

  integ->dt = 0.001;

  integ->frame_update_count = 0;
  integ->prev_frame_time = 0;
  finalize_frame(integ);
  integ->time_accumulator.as_double = 0.0;
  integ->interp_alpha.as_double = 0.0;
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

void finalize_frame(Integrator *integ) {
  vatomic_barrier();
  if (
      vatomic32_compare_exchange(
        &(integ->frame_update_count), 0, integ->particle_count
      ) == 0) {
    integ->time_accumulator.as_double -= integ->dt;
    if (integ->time_accumulator.as_double < integ->dt) {
      integ->time_accumulator.as_double += __min(natime() - integ->prev_frame_time, 0.25);
      integ->interp_alpha.as_double = integ->time_accumulator.as_double / integ->dt;
    }
  }
}

void finalize_particle(Integrator * integ, int index) {
  int k;
  vatomic_barrier();
  if (
      vatomic32_compare_exchange(
        integ->particle_update_count + index, integ->particle_count - 1, 0
      ) == integ->particle_count - 1) {
    /* The following is atomic for the pathological case that all collisions for
     * this particle are re-evaluated before its position can finish updating
     */
    atomic_update_Particle_position(integ->particles + index, integ->dt);
    vatomic32_decrement(&(integ->frame_update_count));
    for (
        k = pair_index(index, index+1, integ->particle_count);
        k < pair_index(index, integ->particle_count, integ->particle_count);
        ++k) {
      vqueue_push(integ->collide_queue, (void *) (integ->pairs + k));
    }
  }
}

void worker_loop(void * args) {
  Integrator * integ = (Integrator *) args;
  /* TODO: Sweep and prune (Also think about data locality).*/
  void * retrieved;
  ParticlePair * cur_pair;

  for (;;) {
    if (vqueue_pop(integ->collide_queue, &retrieved)) {
      cur_pair = (ParticlePair *) retrieved;

      collide(integ, cur_pair);

      vatomic32_increment(integ->particle_update_count + cur_pair->i);
      vatomic32_increment(integ->particle_update_count + cur_pair->j);
      finalize_particle(integ, cur_pair->i);
      finalize_particle(integ, cur_pair->j);

      finalize_frame(integ);
    }
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
