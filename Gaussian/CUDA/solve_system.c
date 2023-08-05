/*!
    \file   solve_system.c
    \brief  Solve a large system of simultaneous equations.
    \author (C) Copyright 2015 by Peter C. Chapin <PChapin@vtc.vsc.edu>
*/

#include <stdlib.h>
#include <stdio.h>
#include <Timer.h>

#include "linear_equations.h"

int main( int argc, char *argv[] )
{
    FILE *input_file;
    int   size;

    if( argc != 2 ) {
        printf( "Error: Expected the name of a system definition file.\n" );
        return EXIT_FAILURE;
    }

    if( (input_file = fopen( argv[1], "r" )) == NULL ) {
        printf("Error: Can not open the system definition file.\n");
        return EXIT_FAILURE;
    }

    // Get the size.
    fscanf( input_file, "%d", &size );

    // Allocate the arrays.
    floating_type *a = MATRIX_MAKE( size );
    floating_type *b = (floating_type *)malloc( size * sizeof( floating_type ) );

    // Get coefficients.
    for( size_t i = 0; i < size; ++i ) {
        for( size_t j = 0; j < size; ++j ) {
            fscanf( input_file, "%f", MATRIX_GET_REF( a, size, i, j ) );  // Nasty type unsafety!
        }
        fscanf( input_file, "%f", &b[i] );  // Here too!
    }
    fclose( input_file );

    Timer stopwatch;

    Timer_initialize( &stopwatch );
    Timer_start( &stopwatch );
    int error = gaussian_solve( size, a, b );
    Timer_stop( &stopwatch );

    if( error ) {
        printf( "System is degenerate\n" );
    }
    else {
        printf( "\nSolution is\n" );
        for( int i = 0; i < size; ++i ) {
            printf( " x(%4d) = %9.5f\n", i, b[i] );
        }

        printf( "\nExecution time = %ld milliseconds\n", Timer_time( &stopwatch ) );
    }

    // Clean up the dynamically allocated space.
    MATRIX_DESTROY( a );
    free( b );
    return EXIT_SUCCESS;
}
