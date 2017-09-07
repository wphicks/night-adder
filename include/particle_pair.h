#pragma once
#include "particle.h"

typedef struct {
  int i;
  int j;
  int pair_index;
  double reduced_mass;
  double restitution;
  double square_sum_radii;
} ParticlePair;

/*
** Initialize a ParticlePair's member values.
** @param pair Pointer to the ParticlePair to initialize.
** @param i Index of first particle in pair
** @param j Index of second particle in pair
** @param count Total number of particles
** @param all_particles Pointer to array of all particles
*/
void init_ParticlePair(
  ParticlePair * pair, int i, int j, int count, Particle * all_particles);
/*
** Return the index corresponding to the pair i, j in count particles.
*/
int pair_index(int i, int j, int count);
