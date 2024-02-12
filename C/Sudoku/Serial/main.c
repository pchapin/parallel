/*! \file    main.c
 *  \brief   Main program of the Sudoku solver.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include <stdlib.h>
#include <stdio.h>

#include "Timer.h"
#include "sudoku.h"

int main( int argc, char **argv )
{
    int exit_status = EXIT_SUCCESS;

    // Check command line.
    if( argc != 2 ) {
        fprintf( stderr, "Usage: %s board_file\n", argv[0] );
        exit_status = EXIT_FAILURE;
    }
    else {
        SolutionManager manager;
        SudokuBoard board;

        // Initialize.
        SolutionManager_construct( &manager );
        SudokuBoard_construct( &board, &manager, 9 );

        // Read the puzzle.
        SudokuBoard_load( &board, argv[1] );
        SudokuBoard_display( &board, stdout );

        // Solve the puzzle.
        printf( "Solving...\n\n" );
        SudokuBoard_solve( &board );
        printf( "\n%lli solutions found.\n", SolutionManager_solution_count( &manager ) );

        // Clean up.
        SudokuBoard_destroy( &board );
        SolutionManager_destroy( &manager );
    }

    return exit_status;
}
