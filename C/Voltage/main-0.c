/*
 * \file main-0.c
 * \brief A serial solution to the voltage field problem described in CIS-4230, Homework #2.
 *
 * This program uses a single thread to compute the voltage field inside a square region of
 * space bounded by several "plates" at different voltages. It thus solves the problem described
 * in CIS-4230, Homework #2 except that it is not multi-threaded. As such it can serve as a
 * baseline against which the multi-threaded versions can be compared.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "Timer.h"
#include "shared.h"

int main_0( void )
{
    int    rc = EXIT_SUCCESS;
    int    iteration_count = 0;
    Timer  stopwatch;
    double temp;
    int    found_big_change;

    Timer_initialize( &stopwatch );
    initialize_workspace( );

    Timer_start( &stopwatch );
    while( 1 ) {
        // Do an iteration.
        found_big_change = FALSE;
        for( int i = 1; i < SIZE - 1; ++i ) {
            for( int j = 1; j < SIZE - 1; ++j ) {
                temp = ( workspace[i - 1][j] +
                         workspace[i + 1][j] +
                         workspace[i][j - 1] +
                         workspace[i][j + 1] ) / 4.0;
                
                if( !found_big_change &&
                      fabs( ( temp - workspace[i][j] ) / workspace[i][j] ) > 1.0E-2 ) {
                    found_big_change = TRUE;
                }
                workspace[i][j] = temp;
            }
        }
        ++iteration_count;

        if( iteration_count % 500 == 0 ) {
            printf( "\rCompleted iteration %d", iteration_count ); fflush( stdout );
        }

        // Is the answer good enough?
        if( !found_big_change ) {
            printf( "\rCompleted iteration %d\n", iteration_count );
            break;
        }
    }
    Timer_stop( &stopwatch );

    // Display the answer.
    printf( "Saving result...\n" );
    save_workspace( );
    printf( "Computation time required: %ld milliseconds\n", Timer_time( &stopwatch ) );

    return rc;
}
