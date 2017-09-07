#pragma once
#include "nathread.h"

#include "vqueue.h"

#include "vector.h"
#include "particle.h"
#include "particle_pair.h"

typedef struct {
  int particle_count; /**< Number of particles */
  Particle * particles; /**< Array of particles */

  int pair_count; /**< Number of particle pairs */
  ParticlePair * pairs; /**< Array of particle pairs */

  vqueue_t collide_queue;
  void * queue_buffer;
  int32_t * update_count;

} Integrator;

/*
** Initialize an integrator which can be used to calculate physics interactions
** for a group of particles.
** @param integ Pointer to the integrator to initialize.
** @param particle_count The number of particles this Integrator will work on.
** @param particles Array of particles on which to perform calculations.
** @see cleanup_Integrator
*/
void init_Integrator(
  Integrator * integ, int particle_count, Particle * particles
);
/*
** Free any memory allocated by init_Integrator
*/
void cleanup_Integrator(Integrator *);

/*
** Check if pair has collided, updating particle velocities as necessary
** @return 1 if particles have collided, otherwise 0
*/
int collide(Integrator * integ, ParticlePair * pair);
