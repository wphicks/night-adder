#pragma once
#include "vector.h"

typedef struct {
  Vec * position;
  Vec * velocity;
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
Particle * create_Particle(double mass, double radius, double restitution);
/*
** Destroy a Particle, freeing allocated memory.
*/
void free_Particle(Particle * old_particle);
