/*
 * \file shared.c
 *
 * \brief Implementation of various helper functions that are shared between the program
 * variants.
 */

#include <stdio.h>
#include "shared.h"

// We assume this array is at least 64 bit aligned. Furthermore we assume that reads and writes
// from 64 bit, aligned data (such as type double) is atomic. These assumptions should be
// verified by consulting the Intel architecture documentation and the compiler documentation.
//
// TODO: Include appropriate citations/documentation here to justify assumptions above!
//
double workspace[SIZE][SIZE];


void initialize_workspace( void )
{
    // Top row initialized to 1.0 volts.
    for( int i = 0; i < SIZE; ++i ) {
        workspace[0][i] = 1.0;
    }

    // Bottom row initialized to 0.0 volts.
    for( int i = 0; i < SIZE; ++i ) {
        workspace[SIZE - 1][i] = 0.0;
    }

    // Left side initialized to 0.25 volts.
    for( int i = 0; i < SIZE; ++i ) {
        workspace[i][0] = 0.25;
    }

    // Right side initialized to 0.75 volts.
    for( int i = 0; i < SIZE; ++i ) {
        workspace[i][SIZE - 1] = 0.75;
    }

    // Zero out the interior of the workspace.
    for( int i = 1; i < SIZE - 1; ++i ) {
        for( int j = 1; j < SIZE - 1; ++j ) {
            workspace[i][j] = 0.0;
        }
    }
}


void save_workspace( void )
{
    FILE *output_file;

    if( ( output_file = fopen( "Voltage-results.txt", "w" ) ) == NULL ) {
        printf( "Unable to open results file!\n" );
    }
    else {
        for( int i = 0; i < SIZE; ++i ) {
            fprintf( output_file, "Row #%d:\n", i );
            for( int j = 0; j < SIZE; ++j ) {
                fprintf( output_file, "\t%10.3E\n", workspace[i][j] );
            }
        }
        fclose( output_file );
    }
}
