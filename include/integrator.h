#pragma once
#include "nathread.h"

#include "vbase.h"
#include "vqueue.h"

#include "nabase.h"
#include "vector.h"
#include "particle.h"
#include "particle_pair.h"

typedef struct {
  int particle_count; /**< Number of particles */
  Particle * particles; /**< Array of particles */

  int pair_count; /**< Number of particle pairs */
  ParticlePair * pairs; /**< Array of particle pairs */

  vqueue_t collide_queue; /**< Queue of pairs to be checked for collisions */
  void * queue_buffer; /**< Memory buffer allocated for collide_queue*/
  int32_t * particle_update_count; /**< Array tracking how many times each
                                     particle has been checked for collisions
                                     this frame */
  int32_t frame_update_count; /**< Tracks how many particles remain to be updated this frame */
  atomic_double time_accumulator; /**< Time waiting to be integrated */

  atomic_double interp_alpha; /**< Interpolation factor between frames */ 

  double dt; /**< Integration time step */
  uint64_t prev_frame_time; /**< Time this frame started */

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
/*
** Continuously integrate simulation in a worker thread.
** This method can be called as many times as desired in various threads to add
** workers to integration.
*/
void worker_loop(void * integrator);
