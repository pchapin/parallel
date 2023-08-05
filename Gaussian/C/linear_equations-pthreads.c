/*!
 * \file   linear_equations-pthreads.c
 * \brief  A gaussian elimination solver.
 * \author (C) Copyright 2018 by Peter C. Chapin <pchapin@vtc.edu>
 *
 * This is the parallel version using pthreads in a "simple" way.
 */

#include <math.h>
#include <string.h>
#include <pthread.h>
#ifdef __GLIBC__
#include <sys/sysinfo.h>
#endif
#include "linear_equations.h"

struct WorkUnit {
    int base_row;      //!< The row which contains the diagonal element we are processing.
    int start_row;     //!< The first row this thread must update.
    int stop_row;      //!< Just past the last row this thread must update.
    int size;          //!< The size of the overall system: 'size' equations with 'size' unknowns.
    floating_type *a;  //!< Pointer to the matrix of coefficients as a linear array.
    floating_type *b;  //!< Pointer to the driving vector.
};

//! Zeros out the column beneath the diagonal element at position (base_row, base_row).
/*!
 * This function is executed by multiple threads with each thread getting a different work
 * unit structure that defines the range of rows to process. Once all the threads have
 * completed their work, the entire column beneath the diagonal element on the base row will
 * have been zeroed.
 */
static void *process_rows( void *raw )
{
    struct WorkUnit *arg = ( struct WorkUnit * )raw;

    // Extract the parameters from the given structure as a notational convenience.
    const int base_row  = arg->base_row;
    const int start_row = arg->start_row;
    const int stop_row  = arg->stop_row;
    const int size      = arg->size;
    floating_type *const a = arg->a;
    floating_type *const b = arg->b;

    // Temporary variable.
    floating_type  m;

    for( int j = start_row; j < stop_row; ++j ) {
        m = MATRIX_GET( a, size, j, base_row ) / MATRIX_GET( a, size, base_row, base_row );
        for( int k = 0; k < size; ++k )
            MATRIX_PUT( a, size, j, k, MATRIX_GET( a, size, j, k ) - m * MATRIX_GET( a, size, base_row, k ) );
        b[j] -= m * b[base_row];
    }
    return NULL;
}


//! Does the elimination step of reducing the system.
static int elimination( int size, floating_type *a, floating_type *b )
{
    floating_type *temp_array = (floating_type *)malloc( size * sizeof(floating_type) );
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

        // Subtract multiples of row i from subsequent rows.
        // Create a team of threads to do this work...
        
        #ifdef __GLIBC__
        int processor_count = get_nprocs( );
        #else
        int processor_count = pthread_num_processors_np( );
        #endif
        //int processor_count = 8;
        
        pthread_t *thread_IDs =
            (pthread_t *)malloc( processor_count * sizeof( pthread_t ) );
        struct WorkUnit *work_units =
            ( struct WorkUnit * )malloc( processor_count * sizeof( struct WorkUnit ) );
        
        for( int thread_counter = 0; thread_counter < processor_count; ++thread_counter ) {
            // Set up the work unit for the current thread.
            int rows_per_processor = ( size - i - 1 ) / processor_count;
            work_units[thread_counter].base_row = i;
            work_units[thread_counter].start_row =
                i + 1 + ( thread_counter * rows_per_processor );
            if( thread_counter == processor_count - 1 ) {
                work_units[thread_counter].stop_row =
                    size;
            }
            else {
                work_units[thread_counter].stop_row =
                    i + 1 + ( ( thread_counter + 1 ) * rows_per_processor );
            }
            work_units[thread_counter].size = size;
            work_units[thread_counter].a = a;
            work_units[thread_counter].b = b;

            // Launch the current thread.
            pthread_create(
                &thread_IDs[thread_counter], NULL, process_rows, &work_units[thread_counter] );
        }

        // Wait for the threads to end.
        for( int thread_counter = 0; thread_counter < processor_count; ++thread_counter ) {
            pthread_join( thread_IDs[thread_counter], NULL );
        }
        free( thread_IDs );
        free( work_units );
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


int gaussian_solve_pthreads( int size, floating_type *a, floating_type *b )
{
    int return_code = elimination( size, a, b );
    if( return_code == 0 )
        return_code = back_substitution( size, a, b );
    return return_code;
}
