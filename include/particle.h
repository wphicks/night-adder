#pragma once
#include "vector.h"

typedef struct {
  Vec position;
  Vec velocity;
  double inv_mass;
  double restitution;
  double radius;
} Particle;

/*
** Initialize a circular particle with set radius, mass, and coefficient of restitution
*/
void init_Particle(Particle * part, double mass, double radius, double restitution);

/*
** Update particle's position based on its velocity.
**
** @param dt The time-step for the update.
*/
void update_Particle_position(Particle * cur_part, double dt);
/*
** Atomically update particle's position based on its velocity.
**
** @param dt The time-step for the update.
*/
void atomic_update_Particle_position(Particle * cur_part, double dt);
