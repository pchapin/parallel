/*!
 * \file   linear_equations.c
 * \brief  A gaussian elimination solver.
 * \author (C) Copyright 2014 by Peter C. Chapin <pchapin@vtc.edu>
 *
 * This is the MPI version of the algorithm.
 */

#include <math.h>
#include <string.h>
#include <mpi.h>

#include "linear_equations.h"

//! Does the elimination step of reducing the system.
static int elimination(
    int number_of_processes, int my_rank, int size, floating_type *a, floating_type *b )
{
    floating_type *temp_array = (floating_type *)malloc( size * sizeof(floating_type) );
    int            i, j, k;
    floating_type  temp, m;
    int            band_width = size / number_of_processes;
    int            my_start, my_end;  // The range of rows to consider: [my_start, my_end).

    // Adjust the bandwidth to account for remainder (if any). The last band is partial.
    if( size % number_of_processes != 0 ) band_width++;

    // Which rows is this process responsible for handling?
    my_start = my_rank * band_width;
    if( my_rank == number_of_processes - 1 ) {
        my_end = size;
    }
    else {
        my_end = my_start + band_width;
    }

    // It is necessary to process all rows so the last row is broadcast back to rank 0.
    for( i = 0; i < size; ++i ) {

        // For now, ignore the swapping. It's tricky to handle properly.
        #ifdef NEVER
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
            return -2;
        }

        // Exchange row i and row k, if necessary.
        if( k != i ) {
            memcpy( temp_array, MATRIX_GET_ROW( a, size, i ), size * sizeof( floating_type ) );
            memcpy( MATRIX_GET_ROW( a, size, i ), MATRIX_GET_ROW( a, size, k ), size * sizeof( floating_type ) );
            memcpy( MATRIX_GET_ROW( a, size, k ), temp_array, size * sizeof( floating_type ) );
            
            // Exchange corresponding elements of b.
            temp = b[i];
            b[i] = b[k];
            b[k] = temp;
        }
        #endif

        // Broadcast the current row.
        MPI_Bcast( MATRIX_GET_ROW( a, size, i ),   // A pointer to the row.
                   size,                           // Number of elements in the row.
                   MPI_DOUBLE,                     // Type of each element.
                   i / band_width,                 // The rank doing the broadcasting.
                   MPI_COMM_WORLD );               // The scope of the broadcast.

        // Broadcast the value in the driving vector.
        MPI_Bcast( &b[i], 1, MPI_DOUBLE, i / band_width, MPI_COMM_WORLD );

        // Subtract multiples of row i from subsequent rows.
        for( j = i + 1; j < size; ++j ) {

            // Only process rows that this process is responsible for handling.
            if( j >= my_start && j < my_end ) {
                m = MATRIX_GET( a, size, j, i ) / MATRIX_GET( a, size, i, i );
                #pragma omp parallel for
                for( k = 0; k < size; ++k )
                    MATRIX_PUT( a, size, j, k, MATRIX_GET( a, size, j, k ) - m * MATRIX_GET( a, size, i, k ) );
                b[j] -= m * b[i];
            }
        }
    }
    return 0;
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


int gaussian_solve( int size, floating_type *a, floating_type *b )
{
    int number_of_processes;
    int my_rank;

    MPI_Comm_size( MPI_COMM_WORLD, &number_of_processes );
    MPI_Comm_rank( MPI_COMM_WORLD, &my_rank );

    int return_code = elimination( number_of_processes, my_rank, size, a, b );

    // Only the rank zero process does back_substitution. The other processes can terminate.
    if( my_rank == 0 && return_code == 0 )
        return_code = back_substitution( size, a, b );
    return return_code;
}
