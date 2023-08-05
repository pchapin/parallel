/*! \file    main.cpp
 *  \brief   Program to compute digits of Pi.
 *  \author  Peter Chapin <pchapin@vtc.edu>
 *
 * See: https://en.wikipedia.org/wiki/Approximations_of_%CF%80. The program below uses Ramanujan's
 * series. This program is just a sketch. It does not deal with the final factor of sqrt(2). It
 * is also highly inefficient since it computes each term of the series from scratch.
 *
 * How can we parallelize this?
 */


#include <cstdlib>
#include <iostream>

#include <Rational.hpp>
#include <VeryLong.hpp>

using namespace std;
using namespace spica;


VeryLong factorial( int k )
{
    if( k == 0 ) return VeryLong::one;

    VeryLong result = VeryLong::one;
    for( int i = 2; i <= k; ++i ) {
        result *= i;
    }

    return result;
}


VeryLong power_factor( int k )
{
    if( k == 0 ) return VeryLong::one;

    VeryLong result = VeryLong::one;
    const VeryLong base = 396;

    for( int i = 1; i <= k; ++i ) {
        result *= base;
        result *= base;
        result *= base;
        result *= base;
    }
    return result;
}


Rational<VeryLong> compute_term( int k )
{
    VeryLong numerator   = factorial( 4 * k ) * (1103 + 26390 * VeryLong( k ) );
    VeryLong k_factorial = factorial( k );
    VeryLong denominator =
        k_factorial * k_factorial * k_factorial * k_factorial * power_factor( k );
    return Rational<VeryLong>( numerator, denominator );
}



int main( int argc, char **argv )
{
    if( argc != 2 ) {
        cout << "Usage: %s <#terms>\n"
                "       where #terms is the number of terms of the Ramanujan series." << endl;
        return EXIT_FAILURE;
    }

    int bound = atoi( argv[1] ) - 1;
    Rational<VeryLong> result;  // Default constructor sets value to zero.
    
    for( int k = 0; k <= bound; ++k ) {
        result += compute_term( k );
    }

    result *= Rational<VeryLong>( 2, 9801 );

    // The result right now still needs to be multiplied by the sqrt(2) to have the right answer.
    cout << inverse( result ) << endl;
    return EXIT_SUCCESS;
}
