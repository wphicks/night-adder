#include <stdlib.h>
#include "vector.h"
#include "particle.h"

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
  init_Vec(&part->prev_position, zero);
  init_Vec(&part->velocity, zero);
}

void update_Particle_position(Particle * cur_part, double dt) {
  copy_Vec(&(cur_part->position), &(cur_part->prev_position));
  Vec update_vec = cur_part->velocity;
  multiply_Vec(&update_vec, dt, &update_vec);
  sum_Vec(&(cur_part->position), &update_vec, &(cur_part->position));
}

void interpolate_Particle_position(
    Particle * part, double alpha, Vec * result) {
  Vec tmp1;
  Vec tmp2;
  multiply_Vec(&(part->prev_position), alpha, &tmp1);
  multiply_Vec(&(part->position), 1.0 - alpha, &tmp2);
  sum_Vec(&tmp1, &tmp2, result);
}
