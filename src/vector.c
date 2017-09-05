#include <stdlib.h>
#include <math.h>
#include "vector.h"
#include "vbase.h"
#include "vatomic.h"

Vec * create_Vec() {
  Vec * new_vec = malloc(sizeof(Vec));
  int i;
  for (i = 0; i < VECDIM; ++i) {
    new_vec->components[i].as_double = 0;
  }
  return new_vec;
}

void free_Vec(Vec * vec){
  free(vec);
}

void sum_Vec(Vec * vec1, Vec * vec2, Vec * result) {
  int i;
  for (i = 0; i < VECDIM; ++i) {
    result->components[i].as_double = (
      vec1->components[i].as_double + vec2->components[i].as_double
    );
  }
}

void atomic_isum_Vec(Vec * vec1, Vec * vec2) {
  int i;
  atomic_double cur_comp;
  atomic_double cur_sum;
  for (i = 0; i < VECDIM; ++i) {
    for (;;) {
      vatomic_barrier();
      cur_comp = vec1->components[i];
      cur_sum.as_double = cur_comp.as_double + vec2->components[i].as_double;
      if (vatomic64_compare_exchange(
            &vec1->components[i].as_int, cur_comp.as_int, cur_sum.as_int
          ) == cur_comp.as_int){
        break;
      }
    }
  }
}

void diff_Vec(Vec * vec1, Vec * vec2, Vec * result) {
  int i;
  for (i = 0; i < VECDIM; ++i) {
    result->components[i].as_double = (
      vec1->components[i].as_double - vec2->components[i].as_double
    );
  }
}

void atomic_idiff_Vec(Vec * vec1, Vec * vec2) {
  int i;
  atomic_double cur_comp;
  atomic_double cur_diff;
  for (i = 0; i < VECDIM; ++i) {
    for (;;) {
      vatomic_barrier();
      cur_comp = vec1->components[i];
      cur_diff.as_double = cur_comp.as_double - vec2->components[i].as_double;
      if (vatomic64_compare_exchange(
            &vec1->components[i].as_int, cur_comp.as_int, cur_diff.as_int
          ) == cur_comp.as_int){
        break;
      }
    }
  }
}

double dot_Vec(Vec * vec1, Vec * vec2) {
  double prod = 0.0;
  int i;
  for (i = 0; i < VECDIM; ++i) {
    prod += vec1->components[i].as_double * vec2->components[i].as_double;
  }
  return prod;
}

double distance_square(Vec * point1, Vec * point2) {
  double result = 0.0;
  int i;
  double comp_diff;
  for (i = 0; i < VECDIM; ++i) {
    comp_diff = (
      point1->components[i].as_double - point2->components[i].as_double
    );
    result += comp_diff * comp_diff;
  }
  return result;
}

double distance(Vec * vec1, Vec * vec2) {
  return sqrt(distance_square(vec1, vec2));
}

double magnitude_square(Vec * vec1) {
  return dot_Vec(vec1, vec1);
}

double magnitude(Vec * vec1) {
  return sqrt(magnitude_square(vec1));
}

void norm_Vec(Vec * vec1, Vec * result) {
  double inv_mag = 1/magnitude(vec1);
  multiply_Vec(vec1, inv_mag, result);
}

void multiply_Vec(Vec * vec1, double scalar, Vec * result) {
  int i;
  for (i = 0; i < VECDIM; ++i) {
    result->components[i].as_double = vec1->components[i].as_double * scalar;
  }
}

void atomic_imultiply_Vec(Vec * vec1, double scalar) {
  int i;
  atomic_double cur_comp;
  atomic_double cur_prod;
  for (i = 0; i < VECDIM; ++i) {
    for (;;) {
      vatomic_barrier();
      cur_comp = vec1->components[i];
      cur_prod.as_double = cur_comp.as_double * scalar;
      if (vatomic64_compare_exchange(
            &vec1->components[i].as_int, cur_comp.as_int, cur_prod.as_int
          ) == cur_comp.as_int){
        break;
      }
    }
  }
}
