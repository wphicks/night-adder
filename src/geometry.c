#include <stdlib.h>
#include <math.h>
#include "geometry.h"

Vec * create_Vec() {
  Vec * new_vec = malloc(sizeof(Vec));
  int i;
  for (i = 0; i < VECDIM; ++i) {
    new_vec->components[i] = 0;
  }
  return new_vec;
}

void free_Vec(Vec * vec){
  free(vec);
}

void sumVec(Vec * vec1, Vec * vec2, Vec * result) {
  int i;
  for (i = 0; i < VECDIM; ++i) {
    result->components[i] = vec1->components[i] + vec2->components[i];
  }
}

void diffVec(Vec * vec1, Vec * vec2, Vec * result) {
  int i;
  for (i = 0; i < VECDIM; ++i) {
    result->components[i] = vec1->components[i] - vec2->components[i];
  }
}

float dotVec(Vec * vec1, Vec * vec2) {
  float prod = 0.0f;
  int i;
  for (i = 0; i < VECDIM; ++i) {
    prod += vec1->components[i] * vec2->components[i];
  }
  return prod;
}

float dist_square(Vec * point1, Vec * point2) {
  float result = 0.0f;
  int i;
  float comp_diff;
  for (i = 0; i < VECDIM; ++i) {
    comp_diff = point1->components[i] - point2->components[i];
    result += comp_diff * comp_diff;
  }
  return result;
}

float dist(Vec * vec1, Vec * vec2){
  return sqrt(dist_square(vec1, vec2));
}

float magnitude_square(Vec * vec1){
  return dotVec(vec1, vec1);
}

float magnitude(Vec * vec1){
  return sqrt(magnitude_square(vec1));
}
