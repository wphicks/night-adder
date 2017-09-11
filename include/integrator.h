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
  atomic_double remaining_time; /**< Remaining time requested for integration*/

  int32_t finished_workers; /**< Tracks how many worker threads have returned */

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
** This function can be called as many times as desired in various threads to
** add workers to integration.
*/
void worker_loop(void * integrator);
/*
** Cause all worker threads to return.
** This function should be called to allow a clean shutdown of all running
** integrator threads. It depends on being given an accurate count of how many
** workers it currently has. If desired, a new group of workers may safely be
** added after this function returns.
*/
void stop_workers(Integrator * integ, int32_t worker_count);
/*
** Request that integrator integrate given interval of time (in seconds).
** Note that this assumes at least one worker_loop has been started to actually
** perform the necessary calculations.
*/
void integrate_interval(Integrator * integ, double interval);
