#pragma once
#include <pthread.h>
#include "geometry.h"
#include "particle.h"

typedef struct {
  int thread_count; /**< Number of worker threads */
  pthread_t * workers; /**< Array of worker threads */
  Vec ** swap_vecs; /**< Temporary vector storage for workers */
  Particle ** particles; /**< Array of pointers to particles */
  int particle_count; /**< Number of particles */
  int pair_count; /**< Number of particle pairs */
  float * square_sum_radii; /**< Array of the square of the sum of radii for all particle pairs */
  float * pair_restitution; /**< Array of the min coefficient of restitution for all particle pairs */
  float * pair_inv_mass; /**< Array of the sum of inverse masses for all pairs */
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
** @return 1 if particles have collided, otherwise 0
*/
int collide(Integrator * integ, int i, int j);
