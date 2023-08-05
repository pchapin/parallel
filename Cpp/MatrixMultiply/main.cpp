/*! \file    main.cpp
    \brief   Main program of parallel matrix multiplication etude.
    \author  Peter C. Chapin <pcc482719@gmail.com>

*/

#include <cstdlib>
#include <iostream>
#include <Timer.hpp>
#include "matrix.hpp"

int main( )
{
    spica::Timer stopwatch1;
    spica::Timer stopwatch2;
    const int size = 1000;
    int return_value = EXIT_SUCCESS;

    try {
        Matrix<float>  A( size, size );
        Matrix<float>  B( size, size );
        Matrix<float> C1( size, size );
        Matrix<float> C2( size, size );

        // Fill matrix A with arbitrary data.
        for( int i = 0; i < size; ++i ) {
            for( int j = 0; j < size; ++j ) {
                A.set_element( i, j, static_cast<float>( i * j ) );
            }
        }

        // Make matrix B a copy of matrix A.
        B = A;

        // Do the interesting computation.
        stopwatch1.start( );
        C1 = A * B;
        stopwatch1.stop( );

        stopwatch2.start( );
        C2 = OpenMPMultiply( A, B );
        stopwatch2.stop( );

        std::cout << "Products computed.\n";
        if( !( C1 == C2 ) ) {
            std::cout << "Pthread Recursive and OpenMP results disagree!\n";
        }
        std::cout << "Pthread Recursive Multiply = " << stopwatch1.time( ) << " milliseconds.\n";
        std::cout << "OpenMP Multiply = " << stopwatch2.time( ) << " milliseconds.\n";
    }
    catch( ... ) {
        std::cout << "An unexpected exception was caught!\n";
        return_value = EXIT_FAILURE;
    }

    return return_value;
}
