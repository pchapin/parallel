/*!
 * \file   gaussian.c
 * \brief  A Gaussian Elimination solver.
 * \author (C) Copyright 2024 by Peter Chapin <pchapin@vermontstate.edu>
 *
 * This is the pthreads version of the algorithm. It uses pthreads in a "simple" way.
 */

#include <math.h>
#include <string.h>
#include <pthread.h>
#if defined(__GLIBC__) || defined(__CYGWIN__)
#include <sys/sysinfo.h>
#endif

#include "gaussian.h"

// For profiling, it is best for all functions to be public.
#define PRIVATE // static
#define PUBLIC

// Ideally `a` would be declared as: `floating_type (*a)[size]`. However, C99 (and C11) can't
// dimension a VLA using a structure member in this way. However, we can cast the type back and
// forth as needed, so we can still use the nice multi-dimensional array syntax inside function
// `process_rows` below.
//
struct WorkUnit {
    size_t base_row;   //!< The row which contains the diagonal element we are processing.
    size_t start_row;  //!< The first row this thread must update.
    size_t stop_row;   //!< Just past the last row this thread must update.
    size_t size;       //!< The size of the overall system: 'size' equations with 'size' unknowns.
    floating_type *a;  //!< Pointer to the matrix of coefficients as a linear array.
    floating_type *b;  //!< Pointer to the driving vector.
};

//! Zeros out the column beneath the diagonal element at position (base_row, base_row).
/*!
 * This function is executed by multiple threads with each thread getting a different work unit
 * structure that defines the range of rows to process. Once all the threads have completed
 * their work, the entire column beneath the diagonal element on the base row will have been
 * zeroed.
 */
PRIVATE void *process_rows( void *raw )
{
    struct WorkUnit *arg = ( struct WorkUnit * )raw;

    // Extract the parameters from the given structure as a notational convenience. We declare
    // these local variables as constants to promise the compiler that we don't intend to change
    // them. We also add `restrict` to the pointers to promise the compiler that the arrays `a`
    // and `b` do not overlap. The compiler will simply trust us on this second point and
    // optimize accordingly.
    //
    const size_t base_row  = arg->base_row;
    const size_t start_row = arg->start_row;
    const size_t stop_row  = arg->stop_row;
    const size_t size      = arg->size;
    floating_type (*const restrict a)[size] = (floating_type (*)[size])arg->a;
    floating_type *const restrict b = arg->b;

    // Temporary variable.
    floating_type  m;

    for( size_t j = start_row; j < stop_row; ++j ) {
        m = a[j][base_row] / a[base_row][base_row];
        for( size_t k = 0; k < size; ++k )
            a[j][k] -= m * a[base_row][k];
        b[j] -= m * b[base_row];
    }
    return NULL;
}


//! Does the elimination step of reducing the system. O(n^3)
PRIVATE enum GaussianResult elimination( size_t size, floating_type (* restrict a)[size], floating_type * restrict b )
{
    //floating_type *temp_array = (floating_type *)malloc( size * sizeof(floating_type) );
    floating_type  temp_array[size];
    size_t         i, j, k;
    floating_type  temp, m;

    #if defined(__GLIBC__) || defined(__CYGWIN__)
    int processor_count = get_nprocs( );
    #else
    int processor_count = pthread_num_processors_np( );
    #endif
    //int processor_count = 8;

    pthread_t *thread_IDs = (pthread_t *)malloc( processor_count * sizeof( pthread_t ) );
    struct WorkUnit *work_units = ( struct WorkUnit * )malloc( processor_count * sizeof( struct WorkUnit ) );

    for( i = 0; i < size - 1; ++i ) {

        // Find the row with the largest value of |a[j][i]|, j = i, ..., n - 1
        k = i;
        m = fabs( a[i][i] );
        for( j = i + 1; j < size; ++j ) {
            if( fabs( a[j][i] ) > m ) {
                k = j;
                m = fabs( a[j][i] );
            }
        }

        // Check for |a[k][i]| zero.
        // TODO: The value 1.0E-6 is arbitrary. A more disciplined value should be used.
        if( fabs( a[k][i] ) <= 1.0E-6 ) {
            //free( temp_array );
            return gaussian_degenerate;
        }

        // Exchange row i and row k, if necessary.
        if( k != i ) {
            memcpy( temp_array, a[i], size * sizeof( floating_type ) );
            memcpy( a[i], a[k], size * sizeof( floating_type ) );
            memcpy( a[k], temp_array, size * sizeof( floating_type ) );
            
            // Exchange corresponding elements of b.
            temp = b[i];
            b[i] = b[k];
            b[k] = temp;
        }

        // Subtract multiples of row i from subsequent rows.

        // Create a team of threads to do this work...
        for( int thread_counter = 0; thread_counter < processor_count; ++thread_counter ) {
            // Set up the work unit for the current thread.
            int rows_per_processor = ( size - i - 1 ) / processor_count;
            work_units[thread_counter].base_row = i;
            work_units[thread_counter].start_row = i + 1 + ( thread_counter * rows_per_processor );
            if( thread_counter == processor_count - 1 ) {
                work_units[thread_counter].stop_row = size;
            }
            else {
                work_units[thread_counter].stop_row = i + 1 + ( ( thread_counter + 1 ) * rows_per_processor );
            }
            work_units[thread_counter].size = size;
            work_units[thread_counter].a = (floating_type *)a;
            work_units[thread_counter].b = b;

            // Launch the current thread.
            pthread_create( &thread_IDs[thread_counter], NULL, process_rows, &work_units[thread_counter] );
        }

        // Wait for the threads to end.
        for( int thread_counter = 0; thread_counter < processor_count; ++thread_counter ) {
            pthread_join( thread_IDs[thread_counter], NULL );
        }

        // This is the code that is now being done in parallel.
        //
        //for( j = i + 1; j < size; ++j ) {
        //    m = a[j][i] / a[i][i];
        //    for( k = 0; k < size; ++k )
        //        a[j][k] -= m * a[i][k];
        //    b[j] -= m * b[i];
        //}
    }

    free( work_units );
    free( thread_IDs );
    //free( temp_array );
    return gaussian_success;
}


//! Does the back substitution step of solving the system. O(n^2)
PRIVATE enum GaussianResult back_substitution( size_t size, floating_type (* restrict a)[size], floating_type * restrict b )
{
    floating_type sum;
    size_t        i, j;
    size_t        counter;

    // We can't count i down from size - 1 to zero (inclusive) because it is unsigned.
    for( counter = 0; counter < size; ++counter ) {
        i = ( size - 1 ) - counter;
        // TODO: The value 1.0E-6 is arbitrary. A more disciplined value should be used.
        if( fabs( a[i][i] ) <= 1.0E-6 ) {
            return gaussian_degenerate;
        }

        sum = b[i];
        for( j = i + 1; j < size; ++j ) {
            sum -= a[i][j] * b[j];
        }
        b[i] = sum / a[i][i];
    }
    return gaussian_success;
}


PUBLIC enum GaussianResult gaussian_solve( size_t size, floating_type (* restrict a)[size], floating_type * restrict b )
{
    // We can deal with a 1x1 system, but not an empty system.
    if( size == 0 ) return gaussian_error;

    enum GaussianResult return_code = elimination( size, a, b );
    if( return_code == gaussian_success )
        return_code = back_substitution( size, a, b );
    return return_code;
}
