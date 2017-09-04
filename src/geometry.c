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

void sum_Vec(Vec * vec1, Vec * vec2, Vec * result) {
  int i;
  for (i = 0; i < VECDIM; ++i) {
    result->components[i] = vec1->components[i] + vec2->components[i];
  }
}

void diff_Vec(Vec * vec1, Vec * vec2, Vec * result) {
  int i;
  for (i = 0; i < VECDIM; ++i) {
    result->components[i] = vec1->components[i] - vec2->components[i];
  }
}

float dot_Vec(Vec * vec1, Vec * vec2) {
  float prod = 0.0f;
  int i;
  for (i = 0; i < VECDIM; ++i) {
    prod += vec1->components[i] * vec2->components[i];
  }
  return prod;
}

float distance_square(Vec * point1, Vec * point2) {
  float result = 0.0f;
  int i;
  float comp_diff;
  for (i = 0; i < VECDIM; ++i) {
    comp_diff = point1->components[i] - point2->components[i];
    result += comp_diff * comp_diff;
  }
  return result;
}

float distance(Vec * vec1, Vec * vec2) {
  return sqrt(distance_square(vec1, vec2));
}

float magnitude_square(Vec * vec1) {
  return dot_Vec(vec1, vec1);
}

float magnitude(Vec * vec1) {
  return sqrt(magnitude_square(vec1));
}

void norm_Vec(Vec * vec1, Vec * result) {
  float inv_mag = 1/magnitude(vec1);
  multiply_Vec(vec1, inv_mag, result);
}

void multiply_Vec(Vec * vec1, float scalar, Vec * result) {
  int i;
  for (i = 0; i < VECDIM; ++i) {
    result->components[i] = vec1->components[i] * scalar;
  }
}
