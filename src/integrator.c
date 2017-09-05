#include <stdlib.h>
#include <pthread.h>
#include "geometry.h"
#include "integrator.h"
#define MIN(a,b) (((a)<(b))?(a):(b))

void calc_square_sum_radii(Integrator * integ);
void calc_restitution_pairs(Integrator * integ);
void calc_mass_pairs(Integrator * integ);

Integrator * create_Integrator(int thread_count, Particle ** particles, int particle_count) {
  int i;
  Integrator * new_int = malloc(sizeof(Integrator));

  new_int->thread_count = thread_count;
  new_int->workers = malloc(thread_count * sizeof(pthread_t));
  new_int->swap_vecs = malloc(2 * thread_count * sizeof(Vec *));
  for(i=0; i < 2 * thread_count; ++i) {
    new_int->swap_vecs[i] = create_Vec();
  }

  new_int->particles = particles;
  new_int->particle_count = particle_count;

  new_int->pair_count = (particle_count - 1) * particle_count / 2;

  new_int->square_sum_radii = malloc(new_int->pair_count * sizeof(float));
  calc_square_sum_radii(new_int);

  new_int->pair_restitution = malloc(new_int->pair_count * sizeof(float));
  calc_restitution_pairs(new_int);

  new_int->pair_reduced_mass = malloc(new_int->pair_count * sizeof(float));
  calc_mass_pairs(new_int);

  return new_int;
}

void free_Integrator(Integrator * old_int) {
  free(old_int->workers);
  free(old_int->square_sum_radii);
  free(old_int->pair_restitution);
  free(old_int->pair_reduced_mass);
  free(old_int);
}

void calc_square_sum_radii(Integrator * integ) {
  float radii_sum;
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
      integ->pair_restitution[pair_index(integ, i, j)] = MIN(
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

float pair_dist_square(Integrator * integ, int i, int j) {
  return distance_square(integ->particles[i]->position, integ->particles[j]->position);
}

int collide(Integrator * integ, int i, int j) {
  Particle * part1 = integ->particles[i];
  Particle * part2 = integ->particles[j];
  float collision_projection;
  float impulse_coefficient;
  float dist2 = distance_square(part1->position, part2->position);
  if (dist2 > integ->square_sum_radii[pair_index(integ, i, j)]) {
    return 0;
  }
  /* TODO: Do following by worker */
  /* Below line needs to be atomic */
  diff_Vec(part2->position, part1->position, *(integ->swap_vecs + 0)); /* Position diff */
  norm_Vec(*(integ->swap_vecs + 0), *(integ->swap_vecs + 0)); /* Collision normal */
  /* Below line needs to be atomic */
  diff_Vec(part2->velocity, part1->velocity, *(integ->swap_vecs + 1)); /* Velocity diff */
  collision_projection = dot_Vec(*(integ->swap_vecs + 0), *(integ->swap_vecs + 1));
  if (collision_projection > 0) { /* Already moving apart */
    return 1;
  }

  impulse_coefficient = (
    (1 + integ->pair_restitution[pair_index(integ, i, j)]) *
    collision_projection *
    integ->pair_reduced_mass[pair_index(integ, i, j)]
  );
  multiply_Vec(*(integ->swap_vecs + 0), impulse_coefficient, *(integ->swap_vecs + 0));
  multiply_Vec(*(integ->swap_vecs + 0), -part2->inv_mass, *(integ->swap_vecs + 1));
  multiply_Vec(*(integ->swap_vecs + 0), part1->inv_mass, *(integ->swap_vecs + 0));
  /* Below line needs to be atomic */
  sum_Vec(part1->velocity, *(integ->swap_vecs + 0), part1->velocity);
  /* Below line needs to be atomic */
  sum_Vec(part2->velocity, *(integ->swap_vecs + 1), part2->velocity);
  return 1;
}
