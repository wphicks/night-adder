#include "geometry.h"
#include "particle.h"

Particle * create_Particle(float mass, float radius, float restitution) {
  Particle * new_part = malloc(sizeof(Particle));
  new_part->position = create_Vec();
  new_part->velocity = create_Vec();
  new_part->inv_mass = 1/mass;
  new_part->radius = radius;
  new_part->restitution = restitution;
}

void free_Particle(Particle * old_particle) {
  free_Vec(old_particle->position);
  free_Vec(old_particle->velocity);
  free(old_particle);
}