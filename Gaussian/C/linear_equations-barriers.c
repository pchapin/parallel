/*!
 * \file   linear_equations-barriers.c
 * \brief  A gaussian elimination solver.
 * \author (C) Copyright 2018 by Peter C. Chapin <pchapin@vtc.edu>
 *
 * This is the parallel version using barriers to reduce thread creation/destruction overhead.
 */

#define _POSIX_C_SOURCE 200112L

#include <math.h>
#include <string.h>
#include <pthread.h>
#ifdef __GLIBC__
#include <sys/sysinfo.h>
#endif
#include "linear_equations.h"

struct WorkUnit {
    int base_row;     // The row being combined with the other rows.
    int start_row;    // The first row in the range we are to process.
    int stop_row;     // The last row in the range we are to process.
    int size;         // The size of the system.
    floating_type *a; // The system (as a one dimensional array).
    floating_type *b; // The driving vector.
    int done;         // =TRUE when there is no more work (this is otherwise an invalid unit).
};


pthread_barrier_t work_ready;
pthread_barrier_t work_finished;


void *row_processor( void *arg )
{
    floating_type m;

    struct WorkUnit *work_unit = (struct WorkUnit *)arg;

    while( 1 ) {
        pthread_barrier_wait( &work_ready );
        if( work_unit->done ) break;

        // Extract the parameters from the given structure as a notational convenience.
        const int base_row     = work_unit->base_row;
        const int start_row    = work_unit->start_row;
        const int stop_row     = work_unit->stop_row;
        const int size         = work_unit->size;
        floating_type *const a = work_unit->a;
        floating_type *const b = work_unit->b;

        for( int j = start_row; j < stop_row; ++j ) {
            m = MATRIX_GET( a, size, j, base_row ) / MATRIX_GET( a, size, base_row, base_row );
            for( int k = 0; k < size; ++k )
                MATRIX_PUT( a, size, j, k, MATRIX_GET( a, size, j, k ) - m * MATRIX_GET( a, size, base_row, k ) );
            b[j] -= m * b[base_row];
        }

        pthread_barrier_wait( &work_finished );
    }
    return NULL;
}


//! Does the elimination step of reducing the system.
static int elimination( int size, floating_type *a, floating_type *b )
{
    floating_type *temp_array = (floating_type *)malloc( size * sizeof( floating_type ) );
    int            i, j, k;
    floating_type  temp, m;

    #ifdef __GLIBC__
    int processor_count = get_nprocs( );
    #else
    int processor_count = pthread_num_processors_np( );
    #endif

    pthread_t *thread_IDs =
        (pthread_t *)malloc( processor_count * sizeof( pthread_t ) );
    struct WorkUnit *work_units =
        ( struct WorkUnit * )malloc( processor_count * sizeof( struct WorkUnit ) );

    // Initialize the synchronization primitives.
    pthread_barrier_init( &work_ready, NULL, processor_count + 1 );
    pthread_barrier_init( &work_finished, NULL, processor_count + 1 );

    // Create the threads.
    for( k = 0; k < processor_count; ++k )
        pthread_create( &thread_IDs[k], NULL, row_processor, &work_units[k] );

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
        if( fabs( MATRIX_GET( a, size, k, i ) ) <= 1.0E-5 ) {
            // TODO: Need to kill the worker threads!
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
        // Direct the existing team of threads...

        // Set up the work units.
        for( int thread_counter = 0; thread_counter < processor_count; ++thread_counter ) {
            int rows_per_processor  = (size - i - 1) / processor_count;
            work_units[thread_counter].base_row  = i;
            work_units[thread_counter].start_row =
                i + 1 + ( thread_counter * rows_per_processor );
            if( thread_counter == processor_count - 1 ) {
                work_units[thread_counter].stop_row =
                    size;
            }
            else {
                work_units[thread_counter].stop_row =
                    i + 1 + ( (thread_counter + 1) * rows_per_processor );
            }
            work_units[thread_counter].size = size;
            work_units[thread_counter].a    = a;
            work_units[thread_counter].b    = b;
            work_units[thread_counter].done = 0;  // Not done.
        }

        // Release the beasts.
        pthread_barrier_wait( &work_ready );

        // Wait for the threads to complete this increment of work.
        pthread_barrier_wait( &work_finished );
    }

    // Tell the threads that there is no more work to do.
    for( int thread_counter = 0; thread_counter < processor_count; ++thread_counter ) {
        work_units[thread_counter].done = 1;
    }
    pthread_barrier_wait( &work_ready );

    // Wait for the threads to end.
    for( int thread_counter = 0; thread_counter < processor_count; ++thread_counter ) {
        pthread_join( thread_IDs[thread_counter], NULL );
    }

    pthread_barrier_destroy( &work_ready );
    pthread_barrier_destroy( &work_finished );

    return 0;
}


//! Does the back substitution step of solving the system.
static int back_substitution( int size, floating_type *a, floating_type *b )
{
    floating_type sum;
    int           i, j;

    for( i = size - 1; i >= 0; --i ) {
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


//
// The following (public) function solves Ax=b using Gaussian elimination. It returns the
// solution in b. The value of *error is -1 if there is a problem with the parameters and -2 if
// the system can't be solved.
//
int gaussian_solve_barriers( int size, floating_type *a, floating_type *b )
{
    int return_code = elimination( size, a, b );
    if( return_code == 0 )
        return_code = back_substitution( size, a, b );
    return return_code;
}
