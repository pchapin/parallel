/*! \file    main.c
 *  \brief   Main program of parallel QuickSort demonstration etude.
 *  \author  Peter C. Chapin <pchapin@vtc.edu>
 */

#include <stdio.h>
#include <stdlib.h>

#include "QuickSort.h"
#include "Timer.h"

int *initialize_workspace( int count )
{
    int *workspace = (int *)malloc( count * sizeof(int) );
    for( int i = 0; i < count; ++i ) {
        workspace[i] = i;
    }

    // This might take a while for large workspaces.
    // std::random_shuffle( workspace, workspace + count );
    for( int i = 0; i < count; ++i ) {
        int random_index = rand( ) % count;
        int temp = workspace[i];
        workspace[i] = workspace[random_index];
        workspace[random_index] = temp;
    }

    return workspace;
}

void check_order( int *first, int *last )
{
    int expected = 0;
    while( first != last ) {
        if( *first != expected ) {
            fprintf( stderr, "\nUnexpected value in sorted sequence!\n" );
            return;
        }
        ++expected;
        ++first;
    }
}

int main( )
{
    Timer stopwatch;
    int  *workspace;
    long  seq_time;
    long  par_time;

    typedef void (*test_function_t)( int *, int * );

    test_function_t test_functions[] = {
        std_sort,
        seq_sort,
        par_sort
    };
    const int test_count = sizeof( test_functions ) / sizeof( test_function_t );

    Timer_initialize( &stopwatch );
    printf( "     size    std    seq    par       S\n" );
    printf( "--------------------------------------\n" );
    for( int size = 5000000; size <= 20000000; size += 1000000 ) {
        printf( "%9d", size );
        for( int i = 0; i < test_count; ++i ) {
            workspace = initialize_workspace( size );
            Timer_reset( &stopwatch );
            Timer_start( &stopwatch );
            test_functions[i]( workspace, workspace + size );
            Timer_stop( &stopwatch );
            check_order( workspace, workspace + size );
            if( i == 1 ) seq_time = Timer_time( &stopwatch );  // A little hackish.
            if( i == 2 ) par_time = Timer_time( &stopwatch );
            printf( "  %5ld", Timer_time( &stopwatch ) );
            free( workspace );
        }
        printf("    %4.2f", (double)seq_time / (double)par_time );
        printf( "\n" );
    }
    return EXIT_SUCCESS;
}
