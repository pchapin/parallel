/*!
 *  \file   CreateSystem.cpp
 *  \brief  Main program of the linear equations creator.
 *  \author (C) Copyright 2024 by Peter Chapin <peter.chapin@vermontstate.edu>
 *
 * This program creates a random system of linear equations in a format that is acceptable to
 * the Gaussian Elimination programs.
 *
 * Build this program using the command:
 *
 *    $ g++ -std=c++17 -o CreateSystem -Wall CreateSystem.cpp
 *
 * Run the program to create a system of a certain size (say 100x100) using the command:
 *
 *    $ ./CreateSystem 100 > 100x100.dat
 *
 */

#include <cstdlib>
#include <iomanip>
#include <iostream>

//! Generate a random double in the range -1.0 < x < 1.0
double generate_value( )
{
    double value = static_cast<double>( std::rand( ) ) / RAND_MAX;
    return (2.0 * value) - 1.0;
}


int main( int argc, char **argv )
{
    int size;

    // Check command line validity.
    if( argc != 2 ) {
        std::cerr << "Usage: " << argv[0] << " size" << std::endl;
        return EXIT_FAILURE;
    }

    // Convert size argument to an integer.
    size = std::atoi( argv[1] );

    // Check the sanity of the size argument.
    // TODO: Check against a reasonable upper bound also?
    if( size <= 0 ) {
        std::cerr << "Invalid system size specified: " << size << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << size << std::endl;

    // For each equation...
    for( int i = 1; i <= size; ++i ) {

        // Output a coefficient for each unknown.
        for( int j = 1; j <= size; ++j ) {
            std::cout << std::setw( 18 ) << std::setprecision( 15 ) << std::fixed << generate_value( ) << std::endl;
        }

        // Output the driving vector value for this equation.
        std::cout << std::setw( 18 ) << std::setprecision( 15 ) << std::fixed << generate_value( ) << std::endl;
    }
    return EXIT_SUCCESS;
}
