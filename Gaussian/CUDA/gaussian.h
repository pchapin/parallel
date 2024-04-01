/*!
 * \file   gaussian.h
 * \brief  Interface to gaussian elimination solver.
 * \author (C) Copyright 2015 by Peter Chapin <spicacality@kelseymountain.org>
 */

#ifndef GAUSSIAN_H
#define GAUSSIAN_H

#include <stdlib.h>

// The GPU we are using only knows how to handle type float (is that really true?)
typedef float floating_type;

// Macros for handling matricies.
// These macros manipulate a linear array as if it was a two dimensional array.
// TODO: Create a matrix abstraction? Or would the overhead of doing so be too great?
#define MATRIX_MAKE( size )  ((floating_type *)malloc( (size) * (size) * sizeof( floating_type ) ))
#define MATRIX_DESTROY( matrix )                       ( free( matrix ) )
#define MATRIX_GET( matrix, size, row, column )        ( (matrix)[(row)*(size) + (column)] )
#define MATRIX_GET_REF( matrix, size, row, column )    (&(matrix)[(row)*(size) + (column)] )
#define MATRIX_GET_ROW( matrix, size, row )            (&(matrix)[(row)*(size)] )
#define MATRIX_PUT( matrix, size, row, column, value ) ( (matrix)[(row)*(size) + (column)] = (value) )

//! Do a Gaussian Elimination using 'a' as the matrix of coefficients and 'b' as the driving vector.
/*!
 * This function returns -1 if there is a problem with the parameters and -2 if the system has no
 * solution and is degenerate. Otherwise the function returns zero and the solution in the array 'b'.
 */

#ifdef __cplusplus
extern "C" {
#endif

int gaussian_solve( int size, floating_type *a, floating_type *b );

#ifdef __cplusplus
}
#endif

#endif
