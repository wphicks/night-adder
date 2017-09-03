#pragma once

#define VECDIM 2

typedef struct {
  float components[VECDIM];
} Vec;

/*
** Create a vector.
** @return A pointer to a newly initialized Vec object.
** @see free_Vec
*/
Vec * create_Vec();

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
void sumVec(Vec * vec1, Vec * vec2, Vec * result);
/*
** Calculate the vector difference vec1 - vec2
** @param vec1 The minuend vector
** @param vec2 The subtrahend vector
** @param result Vector in which to store difference vector
*/
void diffVec(Vec * vec1, Vec * vec2, Vec * result);
/*
** Return the scalar product of two vectors
*/
float dotVec(Vec * vec1, Vec * vec2);

/*
** Return the square of the distance between two points
*/
float dist_square(Vec * point1, Vec * point2);
/*
** Return the distance between two points
*/
float dist(Vec * vec1, Vec * vec2);

/*
** Return the square of the magnitude of a vector
** @return A pointer to a new vector object
*/
float magnitude_square(Vec * vec1);
/*
** Return the magnitude of a vector
** @return A pointer to a new vector object
*/
float magnitude(Vec * vec1);
