#include <stdlib.h>

#include "vbase.h"
#include "vatomic.h"

#include "particle_pair.h"

ParticlePair * create_ParticlePair() {
  ParticlePair * new_pair = malloc(sizeof(ParticlePair));
  init_ParticlePair(new_pair, 0, 1, 2, NULL);
  return new_pair;
}

void init_ParticlePair(
    ParticlePair * pair, int i, int j, int count, Particle * all_particles) {
  double radii_sum;
  if (i > j) { /* Ensure i is always smaller than j */
    i ^= j;
    j ^= i;
    i ^= j;
  }
  pair->i = i;
  pair->j = j;
  pair->pair_index = pair_index(i, j, count);

  if (! all_particles) {

    pair->reduced_mass = 0;
    pair->restitution = 0;
    pair->square_sum_radii = 0;

  } else {

    pair->reduced_mass = 1 / (
      all_particles[pair->i].inv_mass + all_particles[pair->j].inv_mass
    );
    pair->restitution = __min(
      all_particles[i].restitution, all_particles[j].restitution
    );
    radii_sum = all_particles[i].radius + all_particles[j].radius;
    pair->square_sum_radii = radii_sum * radii_sum;

  }
}

void free_ParticlePair(ParticlePair * old_pair){
  free(old_pair);
}

int pair_index(int i, int j, int count){
  if (i < j) {
    return i * count - i * (i + 1) / 2 + j - (i + 1);
  } else {
    return j * count - j * (j + 1) / 2 + i - (j + 1);
  }
}
