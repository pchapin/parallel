/*!
    \file   linear_equations.hpp
    \brief  Interface to the gaussian elimination solver. This is the serial version.
    \author (C) Copyright 2011 by Peter C. Chapin <PChapin@vtc.vsc.edu>
*/

#ifndef LINEAR_EQUATIONS_HPP
#define LINEAR_EQUATIONS_HPP

#include <cassert>
#include <cmath>
#include <boost/scoped_array.hpp>
#include "Matrix.hpp"

//
// The following function does the major work of reducing the system.
//
template< typename FloatingType >
bool elimination( Matrix<FloatingType> &a, FloatingType *b )
{
    // Make sure we are dealing with a square matrix.
    assert( a.row_count( ) == a.col_count( ) );

    std::size_t  size = a.row_count( );
    FloatingType temp;
    FloatingType max;
    FloatingType factor;
    boost::scoped_array< FloatingType > temp_array( new FloatingType[size] );

    // For each row (except the last one)...
    for( std::size_t i = 0; i < size - 1; ++i ) {

        // Find the row with the largest value of |a(j, i)|, j = i, ..., n - 1
        std::size_t k = i;
        max = std::abs( a(i, i) );
        for( std::size_t j = i + 1; j < size; ++j ) {
            if( std::abs( a(j, i) ) > max ) {
                k = j;
                max = std::abs( a(j, i) );
            }
        }

        // Check for |a(k, i)| zero.
        if( std::abs( a(k, i) ) <= 1.0E-6 ) {
            return false;
        }

        // Exchange row i and row k, if necessary.
        if( k != i ) {
            std::memcpy( temp_array.get( ), a.get_row( i ), size * sizeof( FloatingType ) );
            std::memcpy( a.get_row( i ), a.get_row( k ), size * sizeof( FloatingType ) );
            std::memcpy( a.get_row( k ), temp_array.get( ), size * sizeof( FloatingType ) );
            
            // Exchange corresponding elements of b.
            temp = b[i];
            b[i] = b[k];
            b[k] = temp;
        }

        // Subtract multiples of row i from subsequent rows.
        for( std::size_t j = i + 1; j < size; ++j ) {
            factor = a(j, i)/a(i, i);
            for( std::size_t k = 0; k < size; ++k ) a(j, k) -= factor * a(i, k);
            b[j] -= factor * b[i];
        }
    }
    return true;
}


//
// The following function does the back substitution step.
//
template< typename FloatingType >
bool back_substitution( Matrix<FloatingType> &a, FloatingType *b )
{
    // Make sure we are dealing with a square matrix.
    assert( a.row_count( ) == a.col_count( ) );

    std::size_t  size = a.row_count( );
    FloatingType sum;

    for(std::size_t index = 0; index <= size - 1; ++index ) {
        std::size_t i = ( size - 1 ) - index;
        if( std::abs( a(i, i) ) <= 1.0E-6 ) {
            return false;
        }

        sum = b[i];
        for( std::size_t j = i + 1; j < size; ++j ) {
            sum -= a(i, j) * b[j];
        }
        b[i] = sum / a(i, i);
    }
    return true;
}


//! Solve Ax=b using Gaussian elimination.
/*!
 *  The solution is returned in b if the elimination is successful. The values of a and b are
 *  consumed in any case.
 *
 *  \return true if the solution was successfully found and false otherwise. 
 */
template< typename FloatingType >
bool gaussian_solve( Matrix<FloatingType> &a, FloatingType *b )
{
    bool success = elimination( a, b );
    if( !success ) 
        return false;
    else
        return back_substitution( a, b );
}

#endif
