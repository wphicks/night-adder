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
** Create a particle.
**
** Creates a circular particle with a set radius, mass, and coefficient of restitution.
** @return A pointer to a newly initialized Particle object.
** @see free_Particle
*/
Particle * create_Particle();
/*
** Initialize a Particle's members
*/
void init_Particle(Particle * part, double mass, double radius, double restitution);
/*
** Destroy a Particle, freeing allocated memory.
*/
void free_Particle(Particle * old_particle);

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
