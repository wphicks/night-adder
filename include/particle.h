#pragma once
#include "geometry.h"

typedef struct {
  Vec * position;
  Vec * velocity;
  float inv_mass;
  float restitution;
  float radius;
} Particle;

/*
** Create a particle.
**
** Creates a circular particle with a set radius, mass, and coefficient of restitution.
** @return A pointer to a newly initialized Particle object.
** @see free_Particle
*/
Particle * create_Particle(float mass, float radius, float restitution);
/*
** Destroy a Particle, freeing allocated memory.
*/
void free_Particle(Particle * old_particle);
