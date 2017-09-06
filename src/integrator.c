#include <stdlib.h>

#include "vbase.h"
#include "vqueue.h"

#include "nathread.h"
#include "vector.h"
#include "integrator.h"

typedef struct {
  Integrator * integ;
  int index;
} thread_spec;

typedef struct {
  int i;
  int j;
  int pair_index;
} particle_pair;

void calc_square_sum_radii(Integrator * integ);
void calc_restitution_pairs(Integrator * integ);
void calc_mass_pairs(Integrator * integ);

Integrator * create_Integrator(int thread_count, Particle ** particles, int particle_count) {
  int i;
  /* TODO: Allocate cur_pairs from pool */
  particle_pair * cur_pair;
  Integrator * new_int = malloc(sizeof(Integrator));

  new_int->thread_count = thread_count;
  new_int->workers = malloc(thread_count * sizeof(thread_t));
  new_int->swap_vecs = malloc(2 * thread_count * sizeof(Vec *));
  for(i=0; i < 2 * thread_count; ++i) {
    new_int->swap_vecs[i] = create_Vec();
  }

  new_int->particles = particles;
  new_int->particle_count = particle_count;

  new_int->pair_count = (particle_count - 1) * particle_count / 2;

  new_int->square_sum_radii = malloc(new_int->pair_count * sizeof(double));
  calc_square_sum_radii(new_int);

  new_int->pair_restitution = malloc(new_int->pair_count * sizeof(double));
  calc_restitution_pairs(new_int);

  new_int->pair_reduced_mass = malloc(new_int->pair_count * sizeof(double));
  calc_mass_pairs(new_int);

  new_int->queue_buffer = malloc(vqueue_get_bytes_required(new_int->pair_count));
  new_int->collide_queue = vqueue_create(queue_buffer, new_int->pair_count);

  new_int->update_count = malloc(new_int->particle_count * sizeof(int32_t));
  for(i=0; i < integ->particle_count; ++i) {
    for(j=i+1; j < integ->particle_count; ++j) {
      cur_pair = malloc(sizeof(particle_pair));
      cur_pair->i = i;
      cur_pair->j = j;
      cur_pair->pair_index = pair_index(new_int, i, j);
      vqueue_push(new_int->collide_queue, ((void *) cur_pair));
    }
    *(new_int->update_count + i) = 0;
  }

  return new_int;
}

void free_Integrator(Integrator * old_int) {
  free(old_int->workers);
  free(old_int->square_sum_radii);
  free(old_int->pair_restitution);
  free(old_int->pair_reduced_mass);
  free(old_int->queue_buffer);
  free(old_int);
}

void calc_square_sum_radii(Integrator * integ) {
  double radii_sum;
  int i;
  int j;
  for(i=0; i < integ->particle_count; ++i) {
    for(j=i+1; j < integ->particle_count; ++j) {
      radii_sum = integ->particles[i]->radius + integ->particles[j]->radius;
      integ->square_sum_radii[pair_index(integ, i, j)] = radii_sum * radii_sum;
    }
  }
}

void calc_restitution_pairs(Integrator * integ) {
  int i;
  int j;
  for(i=0; i < integ->particle_count; ++i) {
    for(j=i+1; j < integ->particle_count; ++j) {
      integ->pair_restitution[pair_index(integ, i, j)] = __min(
        integ->particles[i]->restitution,
        integ->particles[j]->restitution
      );
    }
  }
}

void calc_mass_pairs(Integrator * integ) {
  int i;
  int j;
  for(i=0; i < integ->particle_count; ++i) {
    for(j=i+1; j < integ->particle_count; ++j) {
      integ->pair_reduced_mass[pair_index(integ, i, j)] = (
        1 / (integ->particles[i]->inv_mass + integ->particles[j]->inv_mass)
      );
    }
  }
}

int pair_index(Integrator * integ, int i, int j) {
  if (i < j) {
    return i * integ->particle_count - i * (i + 1) / 2 + j - (i + 1);
  } else {
    return j * integ->particle_count - j * (j + 1) / 2 + i - (j + 1);
  }
}

double pair_dist_square(Integrator * integ, int i, int j) {
  return distance_square(
    integ->particles[i]->position, integ->particles[j]->position
  );
}

/* TODO: Write update_position */

void finalize_frame(Integrator * integ, int index, int worker_index) {
  vatomic_barrier();
  if (
      vatomic32_compare_exchange(
        integ->update_count + index, integ->particle_count - 1, 0
      ) == spec->integ->particle_count - 1) {
    update_Particle_position(integ->particles[i]);
    /* TODO: Push all pairs with index as i back onto queue */
  }
}

void worker_loop(void * args) {
  thread_spec * spec = (thread_spec *) args;
  /* TODO: Sweep and prune (Also think about data locality).*/
  void * retrieved;
  particle_pair * cur_pair;

  for (;;) {
    vqueue_pop(spec->integ->collide_queue, &retrieved)
    cur_pair = (particle_pair *) retrieved;
    finalize_frame(spec->integ, cur_pair->i, spec->index);
    finalize_frame(spec->integ, cur_pair->j, spec->index);

    collide(spec->integ, cur_pair->i, cur_pair->j, spec->index);

    vatomic32_increment(spec->integ->update_count + cur_pair->i);
    vatomic32_increment(spec->integ->update_count + cur_pair->j);
    vatomic_barrier();
  }

}


int collide(Integrator * integ, int i, int j, int worker_index) {
  Particle * part1 = integ->particles[i];
  Particle * part2 = integ->particles[j];
  double collision_projection;
  double impulse_coefficient;
  double dist2 = distance_square(part1->position, part2->position);
  if (dist2 > integ->square_sum_radii[pair_index(integ, i, j)]) {
    return 0;
  }

  diff_Vec(
    part2->position, part1->position,
    *(integ->swap_vecs + 2 * worker_index)
  ); /* Position diff */
  norm_Vec(
    *(integ->swap_vecs + 2 * worker_index),
    *(integ->swap_vecs + 2 * worker_index)
  ); /* Collision normal */
  diff_Vec(
    part2->velocity, part1->velocity,
    *(integ->swap_vecs + 2 * worker_index + 1)
  ); /* Velocity diff */
  collision_projection = dot_Vec(
    *(integ->swap_vecs + 2 * worker_index),
    *(integ->swap_vecs + 2 * worker_index + 1)
  );

  if (collision_projection > 0) { /* Already moving apart */
    return 1;
  }

  impulse_coefficient = (
    (1 + integ->pair_restitution[pair_index(integ, i, j)]) *
    collision_projection *
    integ->pair_reduced_mass[pair_index(integ, i, j)]
  );

  multiply_Vec(
    *(integ->swap_vecs + 2 * worker_index), impulse_coefficient,
    *(integ->swap_vecs + 2 * worker_index)
  );
  multiply_Vec(
    *(integ->swap_vecs + 2 * worker_index), -part2->inv_mass,
    *(integ->swap_vecs + 2 * worker_index + 1)
  );
  multiply_Vec(
    *(integ->swap_vecs + 2 * worker_index), part1->inv_mass,
    *(integ->swap_vecs + 2 * worker_index)
  );

  atomic_isum_Vec(part1->velocity, *(integ->swap_vecs + 2 * worker_index));
  atomic_isum_Vec(part2->velocity, *(integ->swap_vecs + 2 * worker_index + 1));
  return 1;
}
