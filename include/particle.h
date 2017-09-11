#pragma once
#include "vector.h"

typedef struct {
  Vec prev_position;
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
** Also stores position before update in particle's prev_position member.
** @warning This operation is non-atomic.
**
** @param dt The time-step for the update.
*/
void update_Particle_position(Particle * cur_part, double dt);

/*
** Calculate interpolation between particle's previous and current position.
** @param alpha The blending factor (between 0 and 1) to use for the
** interpolation.
** @param result Pointer to vector in which to store result.
*/
void interpolate_Particle_position(Particle * part, double alpha, Vec * result);
