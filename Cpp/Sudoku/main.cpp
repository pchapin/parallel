/*! \file    main.cpp
    \brief   Main program of the Sudoku solver.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

// Standard C++
#include <cstdlib>
#include <iostream>

// Boost
#include <boost/thread.hpp>

// Application support
#include <Timer.hpp>

#include "sudoku.hpp"

int main( int argc, char **argv )
{
    int exit_status = EXIT_SUCCESS;

    // Check command line.
    if( argc != 2 ) {
        std::cerr << "Usage: " << argv[0] << " board_file\n";
        exit_status = EXIT_FAILURE;
    }
    else {
        try {
            SolutionManager manager;
            SudokuBoard board(&manager, 9);
            board.load( argv[1] );
            board.display( std::cout );
            std::cout << "Solving...\n\n";
            board.solve( );
            std::cout << "\n" << manager.solution_count( ) << " solutions found.\n";
        }
        catch( SudokuBoard::Error &e ) {
            std::cout << "Sudoku exception: " << e.what( ) << "\n";
            exit_status = EXIT_FAILURE;
        }
    }

    return exit_status;
}
