/*! \file    main-1.c
 *  \brief   A multi-threaded version of the voltage field solver.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#if defined(__GLIBC__) || defined(__CYGWIN__)
#include <sys/sysinfo.h>
#endif
#include "Timer.h"
#include "shared.h"

// The barriers need to be global so that all threads can see them.
static pthread_barrier_t pass_barrier;
static pthread_barrier_t evaluation_barrier;
static int good_enough = FALSE;

struct WorkUnit {
    int start_row;  // The first row for the thread to process.
    int stop_row;   // Just past the last row for the thread to process.
    int found_big_change; // Used to indicate if more work is needed.
};


static void *row_processor( void *raw )
{
    struct WorkUnit *arg = ( struct WorkUnit * )raw;
    int    start_row = arg->start_row;
    int    stop_row  = arg->stop_row;
    double temp;

    while( 1 ) {
        arg->found_big_change = FALSE;
        for( int i = start_row; i < stop_row; ++i ) {
            for( int j = 1; j < SIZE - 1; ++j ) {
                temp = ( workspace[i - 1][j] + workspace[i + 1][j] + workspace[i][j - 1] + workspace[i][j + 1] ) / 4.0;
                if( !arg->found_big_change && fabs( ( temp - workspace[i][j] ) / workspace[i][j] ) > 1.0E-2 ) {
                    arg->found_big_change = TRUE;
                }
                workspace[i][j] = temp;
            }
        }
        pthread_barrier_wait( &pass_barrier );
        pthread_barrier_wait( &evaluation_barrier );
        if( good_enough ) break;
    }

    return NULL;
}


int main_1( void )
{
    int rc = EXIT_SUCCESS;
    int iteration_count = 0;
    int found_big_change;

    initialize_workspace( );

    // Figure out how many threads to use.
    #if defined(__GLIBC__) || defined(__CYGWIN__)
    int processor_count = get_nprocs( );
    #else
    int processor_count = pthread_num_processors_np( );
    #endif

    // Initialize barriers.
    pthread_barrier_init( &pass_barrier, NULL, processor_count + 1 );
    pthread_barrier_init( &evaluation_barrier, NULL, processor_count + 1 );

    // Split problem into subproblems (let each thread work on a subset of the rows?)
    struct WorkUnit *work_units = ( struct WorkUnit * )malloc( processor_count * sizeof( struct WorkUnit ) );
    int rows_per_processor = ( SIZE - 2 ) / processor_count;
    for( int i = 0; i < processor_count; ++i ) {
        work_units[i].start_row = 1 + i*rows_per_processor;
        if( i == processor_count - 1 ) {
            work_units[i].stop_row = SIZE - 1;
        }
        else {
            work_units[i].stop_row = 1 + ( i + 1 )*rows_per_processor;
        }
    }

    // Start the threads.
    pthread_t *thread_IDs = (pthread_t *)malloc( processor_count * sizeof( pthread_t ) );
    Timer stopwatch;
    Timer_initialize( &stopwatch );
    Timer_start( &stopwatch );
    for( int i = 0; i < processor_count; ++i ) {
        pthread_create( &thread_IDs[i], NULL, row_processor, &work_units[i] );
    }

    while( 1 ) {
        // Wait on a barrier until the threads complete this pass.
        pthread_barrier_wait( &pass_barrier );
        ++iteration_count;

        if( iteration_count % 500 == 0 ) {
            printf( "\rCompleted iteration %d", iteration_count ); fflush( stdout );
        }

        // Evaluate: is the answer good enough?
        // If the answer is ok, set a global flag to tell the other threads to end.
        found_big_change = FALSE;
        for( int i = 0; i < processor_count; ++i ) {
            if( work_units[i].found_big_change ) found_big_change = TRUE;
        }
        if( !found_big_change ) {
            printf( "\rCompleted iteration %d\n", iteration_count );
            good_enough = TRUE;
            pthread_barrier_wait( &evaluation_barrier );
            break;
        }

        // Ensure the other threads don't progress until evaluation is complete.
        pthread_barrier_wait( &evaluation_barrier );
    }

    // Join with the threads.
    for( int i = 0; i < processor_count; ++i ) {
        pthread_join( thread_IDs[i], NULL );
    }
    Timer_stop( &stopwatch );

    free( thread_IDs );
    free( work_units );

    // Destroy the barriers.
    pthread_barrier_destroy( &evaluation_barrier );
    pthread_barrier_destroy( &pass_barrier );

    // Display the answer.
    printf( "Saving result...\n" );
    save_workspace( );
    printf( "Computation time required: %ld milliseconds\n", Timer_time( &stopwatch ) );

    return rc;
}
