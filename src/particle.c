#include <stdlib.h>
#include "vector.h"
#include "particle.h"

Particle * create_Particle(double mass, double radius, double restitution) {
  Particle * new_part = malloc(sizeof(Particle));
  init_Particle(new_part, 0, 0, 0);
  return new_part;
}

void init_Particle(Particle * part, double mass, double radius, double restitution) {
  double zero[VECDIM];
  int i;
  for (i=0; i < VECDIM; ++i) {
    zero[i] = 0;
  }
  if(mass > 0) {
    part->inv_mass = 1/mass;
  } else {
    part->inv_mass = 0;
  }
  part->radius = radius;
  part->restitution = restitution;
  init_Vec(&part->position, zero);
  init_Vec(&part->velocity, zero);
}

void free_Particle(Particle * old_particle) {
  free(old_particle);
}

void update_Particle_position(Particle * cur_part, double dt) {
  Vec update_vec = cur_part->velocity;
  multiply_Vec(&update_vec, dt, &update_vec);
  sum_Vec(&(cur_part->position), &update_vec, &(cur_part->position));
}

void atomic_update_Particle_position(Particle * cur_part, double dt) {
  Vec update_vec = cur_part->velocity;
  multiply_Vec(&update_vec, dt, &update_vec);
  atomic_isum_Vec(&(cur_part->position), &update_vec);
}
