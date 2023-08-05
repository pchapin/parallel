/*!
 * \file   linear_equations.h
 * \brief  Interface to linear equation solver.
 * \author (C) Copyright 2014 by Peter C. Chapin <PChapin@vtc.vsc.edu>
 */

#ifndef LINEAR_EQUATIONS_H
#define LINEAR_EQUATIONS_H

#include <stdlib.h>

typedef double floating_type;

// Macros for handling matricies.
// These macros manipulate a linear array as if it was a two dimensional array.
// TODO: Create a matrix abstraction? Or would the overhead of doing so be too great?
#define MATRIX_MAKE( size )  ((floating_type *)malloc( (size) * (size) * sizeof( floating_type ) ))
#define MATRIX_DESTROY( matrix )                       ( free( matrix ) )
#define MATRIX_GET( matrix, size, row, column )        ( (matrix)[(row)*(size) + (column)] )
#define MATRIX_GET_REF( matrix, size, row, column )    (&(matrix)[(row)*(size) + (column)] )
#define MATRIX_GET_ROW( matrix, size, row )            (&(matrix)[(row)*(size)] )
#define MATRIX_PUT( matrix, size, row, column, value ) ( (matrix)[(row)*(size) + (column)] = (value) )

//! Gaussian Elimination using 'a' as the matrix of coefficients and 'b' as the driving vector.
/*!
 * This function returns -1 if there is a problem with the parameters and -2 if the system has no
 * solution and is degenerate. Otherwise the function returns zero and the solution in the array 'b'.
 */
int gaussian_solve( int size, floating_type *a, floating_type *b );

#endif
