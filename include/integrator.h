#pragma once
#include "nathread.h"

#include "vqueue.h"

#include "vector.h"
#include "particle.h"

typedef struct {
  int thread_count; /**< Number of worker threads */
  thread_t * workers; /**< Array of worker threads */
  Vec ** swap_vecs; /**< Temporary vector storage for workers */
  Particle ** particles; /**< Array of pointers to particles */
  int particle_count; /**< Number of particles */
  int pair_count; /**< Number of particle pairs */

  double * square_sum_radii; /**< Array of the square of the sum of radii for all particle pairs */
  double * pair_restitution; /**< Array of the min coefficient of restitution for all particle pairs */
  double * pair_reduced_mass; /**< Array of the sum of reduced masses for all pairs */

  vqueue collide_queue;
  void * queue_buffer;
  int32_t * update_count;
} Integrator;

/*
** Create an integrator which can be used to calculate physics interactions for
** a group of particles.
** @return A pointer to a newly initialized Integrator object.
** @param threadcount The number of threads to use for this Integrator.
** @see free_Integrator
*/
Integrator * create_Integrator(int thread_count, Particle ** particles, int particle_count);
/*
** Destroy an Integrator, freeing allocated memory.
*/
void free_Integrator(Integrator *);

/*
** Return index for retrieving information about particle pairs from 1D arrays
** @param i Index of first particle in pair
** @param j Index of second particle in pair
** @return Index of pair information in 1D array
*/
int pair_index(Integrator * integ, int i, int j);

/*
** Check if two particles have collided, updating their velocities as necessary
** @param i Index of first particle in pair
** @param j Index of second particle in pair
** @param worker_index Index of worker thread performing calculation
** @return 1 if particles have collided, otherwise 0
*/
int collide(Integrator * integ, int i, int j, int worker_index);
