#pragma once

#include "nabase.h"

#define VECDIM 2

typedef struct {
  atomic_double components[VECDIM];
} Vec;

/*
** Create a vector.
** @return A pointer to a newly initialized Vec struct.
** @see free_Vec
*/
Vec * create_Vec();
/*
** Initialize a vector's components to given values
*/
void init_Vec(Vec * vec, double components[VECDIM]);
/*
** Destroy a vector, freeing allocated memory.
*/
void free_Vec(Vec * vec);

/*
** Calculate the vector sum of two vectors
** @param vec1 One vector to be summed
** @param vec2 Second vector to be summed
** @param result Vector in which to store result
*/
void sum_Vec(Vec * vec1, Vec * vec2, Vec * result);
/*
** Atomically add vectors in-place
**
** @warning Atomicity is only guaranteed per-component, but this is still
** valuable, since it guarantees that if V+=U and V+=W are both being carried
** out at the same time, the resulting final V' will be equal to V + U + W
**
** @param vec1 Vector to be replaced by sum of both vectors
** @param vec2 Vector to be added to first vector
*/
void atomic_isum_Vec(Vec * vec1, Vec * vec2);
/*
** Calculate the vector difference vec1 - vec2
** @param vec1 The minuend vector
** @param vec2 The subtrahend vector
** @param result Vector in which to store difference vector
*/
void diff_Vec(Vec * vec1, Vec * vec2, Vec * result);
/*
** Atomically subtract vectors in-place
**
** @warning Atomicity is only guaranteed per-component, but this is still
** valuable, since it guarantees that if V-=U and V-=W are both being carried
** out at the same time, the resulting final V' will be equal to V - U - W
**
** @param vec1 Vector to be replaced by difference of itself and second vector
** @param vec2 Vector to be added to first vector
*/
void atomic_idiff_Vec(Vec * vec1, Vec * vec2);
/*
** Return the scalar product of two vectors
*/
double dot_Vec(Vec * vec1, Vec * vec2);

/*
** Return the square of the distance between two points
*/
double distance_square(Vec * point1, Vec * point2);
/*
** Return the distance between two points
*/
double distance(Vec * vec1, Vec * vec2);

/*
** Return the square of the magnitude of a vector
** @return A pointer to a new vector object
*/
double magnitude_square(Vec * vec1);
/*
** Return the magnitude of a vector
** @return A pointer to a new vector object
*/
double magnitude(Vec * vec1);
/*
** Calculate normalized vector
** @param vec1 The vector to normalize
** @param result Vector in which to store difference vector
*/
void norm_Vec(Vec * vec1, Vec * result);
/*
** Calculate the vector scaled by a scalar
** @param vec1 The vector to be scaled
** @param scalar The scalar by which to multiply the vector
** @param result Vector in which to store result
*/
void multiply_Vec(Vec * vec1, double scalar, Vec * result);
/*
** Atomically scale vector in-place
**
** @warning Atomicity is only guaranteed per-component, but this is still
** valuable, since it guarantees that if V*=x and V*=y are both being carried
** out at the same time, the resulting final V' will be equal to V*x*y
**
** @param vec1 Vector to be scaled in-place
** @param scalar The scalar by which to multiply the vector
*/
void atomic_imultiply_Vec(Vec * vec1, double scalar);
