/*!
 * \file   linear_equations.c
 * \brief  A gaussian elimination solver.
 * \author (C) Copyright 2014 by Peter C. Chapin <PChapin@vtc.vsc.edu>
 *
 * This is the serial version of the algorithm.
 */

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <cuda_runtime.h>
#include "linear_equations.h"

//! Does the elimination step of reducing the system.
__global__ void elimination
  ( int size, floating_type *a, floating_type *b, floating_type *temp_array, int *status )
{
    int            i, j, k;
    floating_type  temp, m;

    for( i = 0; i < size - 1; ++i ) {

        // Find the row with the largest value of |a[j][i]|, j = i, ..., n - 1
        k = i;
        m = fabs( MATRIX_GET( a, size, i, i ) );
        for( j = i + 1; j < size; ++j ) {
            if( fabs( MATRIX_GET( a, size, j, i ) ) > m ) {
                k = j;
                m = fabs( MATRIX_GET( a, size, j, i ) );
            }
        }

        // Check for |a[k][i]| zero.
        if( fabs( MATRIX_GET( a, size, k, i ) ) <= 1.0E-6 ) {
            *status= -2;
            return;
        }

        // Exchange row i and row k, if necessary.
        if( k != i ) {
            for( j = 0; j < size; ++j ) temp_array[j] = MATRIX_GET( a, size, i, j );
            for( j = 0; j < size; ++j ) MATRIX_PUT( a, size, i, j, MATRIX_GET( a, size, k, j ) );
            for( j = 0; j < size; ++j ) MATRIX_PUT( a, size, k, j, temp_array[j] );

            // Exchange corresponding elements of b.
            temp = b[i];
            b[i] = b[k];
            b[k] = temp;
        }

        // Subtract multiples of row i from subsequent rows.
        for( j = i + 1; j < size; ++j ) {
            m = MATRIX_GET( a, size, j, i ) / MATRIX_GET( a, size, i, i );
            for( k = 0; k < size; ++k )
                MATRIX_PUT( a, size, j, k, MATRIX_GET( a, size, j, k ) - m * MATRIX_GET( a, size, i, k ) );
            b[j] -= m * b[i];
        }
    }
    *status = 0;
    return;
}


//! Does the back substitution step of solving the system.
static int back_substitution( int size, floating_type *a, floating_type *b )
{
    floating_type sum;
    int           i, j;

    for( i = size - 1; i >=0; --i ) {
        if( fabs( MATRIX_GET( a, size, i, i ) ) <= 1.0E-6 ) {
            return -2;
        }

        sum = b[i];
        for( j = i + 1; j < size; ++j ) {
            sum -= MATRIX_GET( a, size, i, j ) * b[j];
        }
        b[i] = sum / MATRIX_GET( a, size, i, i );
    }
    return 0;
}


extern "C" int gaussian_solve( int size, floating_type *a, floating_type *b )
{
    int            return_code;
    floating_type *dev_temp_array;
    int           *dev_return_code;
    floating_type *dev_a;
    floating_type *dev_b;

    cudaMalloc( (void **)&dev_a, size * size * sizeof( floating_type ) );
    cudaMalloc( (void **)&dev_b, size * sizeof( floating_type ) );
    cudaMalloc( (void **)&dev_temp_array, size * sizeof( floating_type ) );
    cudaMalloc( (void **)&dev_return_code, sizeof( int ) );

    cudaMemcpy( dev_a, a, size * size * sizeof( floating_type ), cudaMemcpyHostToDevice );
    cudaMemcpy( dev_b, b, size * sizeof( floating_type ), cudaMemcpyHostToDevice );

    elimination<<<1, 1>>>( size, dev_a, dev_b, dev_temp_array, dev_return_code );
    cudaError_t result = cudaGetLastError( );
    if( result != cudaSuccess ) {
        printf( "Failure invoking the elimination kernel! (%s)\n", cudaGetErrorString( result ) );
    }

    cudaMemcpy( &return_code, dev_return_code, sizeof( int ), cudaMemcpyDeviceToHost );
    if( return_code == 0 ) {
        cudaMemcpy( a, dev_a, size * size * sizeof( floating_type ), cudaMemcpyDeviceToHost );
        cudaMemcpy( b, dev_b, size * sizeof( floating_type ), cudaMemcpyDeviceToHost );
        return_code = back_substitution( size, a, b );
    }
    cudaFree( dev_a );
    cudaFree( dev_b );
    cudaFree( dev_temp_array );
    cudaFree( dev_return_code );
    return return_code;
}
