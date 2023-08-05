/*!
    \file   solve_system.c
    \brief  Solve a large system of simultaneous equations.
    \author (C) Copyright 2011 by Peter C. Chapin <PChapin@vtc.vsc.edu>
*/

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <Timer.hpp>

// Select the serial or parallel version as desired...
// #include "linear_equations.hpp"
#include "linear_equationsp.hpp"

using namespace std;

int main( int argc, char *argv[] )
{
    size_t size;

    if( argc != 2 ) {
        cout << "Error: Expected the name of a system definition file.\n";
        return EXIT_FAILURE;
    }

    ifstream input_file( argv[1] );
    if( !input_file ) {
        printf("Error: Can not open the system definition file.\n");
        return EXIT_FAILURE;
    }

    // Get the size.
    input_file >> size;

    // Allocate the arrays.
    Matrix<float> a( size, size );
    boost::scoped_array<float> b( new float[size] );

    // Get coefficients.
    for( size_t i = 0; i < size; ++i ) {
        for( size_t j = 0; j < size; ++j ) {
            input_file >> a(i, j);
        }
        input_file >> b[i];
    }
    input_file.close( );

    spica::Timer stopwatch;
    stopwatch.start( );
    bool success = gaussian_solve( a, b.get( ) );
    stopwatch.stop( );

    if( !success ) {
        cout << "System is degenerate\n";
    }
    else {
        cout << "\nSolution is\n";
        for( size_t i = 0; i < size; ++i ) {
            cout << " x(" << setw(4) << i << ") = "
                 << setw(9) << std::fixed << std::setprecision(5) << b[i] << "\n";
        }

        cout << "\nExecution time = " << stopwatch.time( ) << " milliseconds\n";
    }

    return EXIT_SUCCESS;
}
