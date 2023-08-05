/*!
    \file   linear_equations.hpp
    \brief  Interface to the gaussian elimination solver. This is the parallel version.
    \author (C) Copyright 2011 by Peter C. Chapin <PChapin@vtc.vsc.edu>
*/

#ifndef LINEAR_EQUATIONS_HPP
#define LINEAR_EQUATIONS_HPP

#include <cassert>
#include <cmath>
#include <boost/scoped_array.hpp>
#include "Matrix.hpp"
#include "ThreadPool.hpp"

// The following structure gathers together parameters needed by the row processor. The
// ThreadPool currently can't handle work functions that take a large number of parameters
// so this hack is necessary until ThreadPool is enhanced.
//
template< typename FloatingType >
struct RowProcessorParameters {
    std::size_t i;      // Index of "source" row.
    std::size_t start;  // Index of first row to process.
    std::size_t end;    // Index just past the last row to process.
    std::size_t size;   // The size of the system (not the number of rows to process!).
    Matrix<FloatingType> *a;  // Points at the matrix of coefficients.
    FloatingType         *b;  // Points at the driving vector.
};


//
// The following function processes a set of rows.
//
template< typename FloatingType >
void row_processor( RowProcessorParameters<FloatingType> *row_set )
{
    // Convenient aliases...
    Matrix<FloatingType> &a = *row_set->a;  // Gives me easy access to operator() in Matrix.
    FloatingType         *b =  row_set->b;  // For symmetry with 'a' above.
    std::size_t           i =  row_set->i;  // Keeps the code below uncluttered.

    FloatingType factor;

    // Subtract multiples of row i from subsequent rows.
    for( std::size_t j = row_set->start; j < row_set->end; ++j ) {
        factor = a(j, i)/a(i, i);
        for( std::size_t k = 0; k < row_set->size; ++k ) a(j, k) -= factor * a(i, k);
        b[j] -= factor * b[i];
    }
}


//
// The following function does the major work of reducing the system.
//
template< typename FloatingType >
bool elimination( Matrix<FloatingType> &a, FloatingType *b )
{
    // Make sure we are dealing with a square matrix.
    assert( a.row_count( ) == a.col_count( ) );

    // It might make more sense to create the thread pool in main(). However, in this case I
    // want to use the same main() for both the serial and parallel versions so I'm motivated
    // to keep the thread pool out of that function.
    //
    spica::ThreadPool threads;
    boost::scoped_array< RowProcessorParameters<FloatingType> >
        parameters_array( new RowProcessorParameters<FloatingType>[ threads.count( ) ] );
    boost::scoped_array< spica::ThreadPool::threadid_t >
        threadID_array( new spica::ThreadPool::threadid_t[ threads.count( ) ] );

    std::size_t  size = a.row_count( );
    FloatingType temp;
    FloatingType max;
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

        // Prepare threads to handle rows i + 1 .. size - 1. Each thread handles a subrange of rows.
        for( int thread_number = 0; thread_number < threads.count( ); ++thread_number ) {

            std::size_t rows_per_thread = (size - i - 1) / threads.count( );
            parameters_array[thread_number].i = i;
            parameters_array[thread_number].start = i + 1 + ( thread_number * rows_per_thread );
            if( thread_number == threads.count( ) - 1 )
                parameters_array[thread_number].end = size;
            else
                parameters_array[thread_number].end = i + 1 + ( (thread_number + 1) * rows_per_thread );
            parameters_array[thread_number].size = size;
            parameters_array[thread_number].a = &a;
            parameters_array[thread_number].b = b;
        }

        // Start the threads.
        for( int thread_number = 0; thread_number < threads.count( ); ++thread_number ) {
            threadID_array[thread_number] =
                threads.start_work( row_processor<FloatingType>, &parameters_array[thread_number] );
        }

        // Wait for the threads to finish.
        for( int thread_number = 0; thread_number < threads.count( ); ++thread_number ) {
            threads.work_result( threadID_array[thread_number] );
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
