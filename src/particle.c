#include <stdlib.h>
#include "vector.h"
#include "particle.h"

Particle * create_Particle(double mass, double radius, double restitution) {
  Particle * new_part = malloc(sizeof(Particle));
  new_part->position = create_Vec();
  new_part->velocity = create_Vec();
  new_part->inv_mass = 1/mass;
  new_part->radius = radius;
  new_part->restitution = restitution;
  return new_part;
}

void free_Particle(Particle * old_particle) {
  free_Vec(old_particle->position);
  free_Vec(old_particle->velocity);
  free(old_particle);
}

void update_Particle_position(Particle * cur_part, double dt) {
  Vec update_vec = *(cur_part->velocity);
  multiply_Vec(&update_vec, dt, &update_vec);
  sum_Vec(cur_part->position, &update_vec, cur_part->position);
}

void atomic_update_Particle_position(Particle * cur_part, double dt) {
  Vec update_vec = *(cur_part->velocity);
  multiply_Vec(&update_vec, dt, &update_vec);
  atomic_isum_Vec(cur_part->position, &update_vec);
}
