/*! \file    main.cpp
    \brief   Main program of the parallel find demonstration application.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

    This program implements a parallel version of std::find and then does a benchmark test of
    its performance as compared to the the std::find in the implementation's library. This
    version implicitly manages the threads using TBB.
*/

// Standard C++
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <vector>

// TBB
#include <tbb/task_scheduler_init.h>
#include <tbb/blocked_range.h>
#include <tbb/parallel_reduce.h>

// Application support
#include <Timer.hpp>


namespace par {

    // This structure encapsulates the results returned by a single thread.
    template< typename RandomAccess, typename T >
    struct FindHelper {
        const T *search_item;        // Points at time we are searching for.
        bool found;                  // =true if the item has been found.
        RandomAccess item_iterator;  // Points at the item if found == true.

        // Constructor to initialize primary FindHelper object.
        FindHelper( const T *find_me )
            : search_item( find_me ), found( false )
        { }

        // Splitting constructor prepares "session."
        FindHelper( FindHelper &other, tbb::split )
            : search_item( other.search_item ), found( false )
        { }

        // Executes find operation over given range.
        void operator( )( const tbb::blocked_range< RandomAccess > &range );

        // Merge results of another FindHelper
        void join( FindHelper &other );
    };

    // Helper function that is executed by each thread (perhaps more than once by a thread).
    template< typename RandomAccess, typename T >
    void FindHelper< RandomAccess, T >::operator( )( const tbb::blocked_range< RandomAccess > &range )
    {
        // If item already found (in previous run), ignore this invocation.
        if( found ) return;

        // Use the standard library find to search this range.
        item_iterator = std::find( range.begin( ), range.end( ), *search_item );
        if( item_iterator != range.end( ) ) found = true;
    }

    template< typename RandomAccess, typename T >
    void FindHelper< RandomAccess, T >::join( FindHelper &other )
    {
        // If we found the item, ignore the other helper.
        if( found ) return;

        if( other.found ) {
            item_iterator = other.item_iterator;
            found = true;
        }
    }


    template< typename RandomAccess, typename T >
    RandomAccess find( RandomAccess first, RandomAccess last, const T &value )
    {
        // Assume we don't find the item.
        RandomAccess overall_result = last;
        FindHelper< RandomAccess, T > primary_helper( &value );

        tbb::parallel_reduce(
            tbb::blocked_range< RandomAccess >( first, last, 100000 ), primary_helper );

        if( primary_helper.found )
            overall_result = primary_helper.item_iterator;

        return overall_result;
    }

}

int main( )
{
    tbb::task_scheduler_init tbb_init;
    spica::Timer stopwatch1, stopwatch2;
    std::vector< int > my_vector;
    
    std::cout << " Length    std    par  Speedup\n";
    std::cout << "==============================\n";
    for( int length = 500000; length <= 3000000; length += 100000 ) {
        std::vector< int >::iterator p1;
        std::vector< int >::iterator p2;
        my_vector.clear( );

        for( int i = 0; i < length; ++i )
            my_vector.push_back( i );

        stopwatch1.reset( );
        stopwatch1.start( );
        for( int i = 0; i < 1000; ++i )
            p1 = std::find( my_vector.begin(), my_vector.end(), length );
        stopwatch1.stop( );

        stopwatch2.reset( );
        stopwatch2.start( );
        for( int i = 0; i < 1000; ++i )
            p2 = par::find( my_vector.begin(), my_vector.end(), length );
        stopwatch2.stop( );

        if( p2 != my_vector.end() ) {
            std::cout << "par::find failed!\n";
        }
        else {
            std::cout << std::setw(7) << length             << ": ";
            std::cout << std::setw(5) << stopwatch1.time( ) << "  ";
            std::cout << std::setw(5) << stopwatch2.time( ) << "  ";
            std::cout << std::showpoint << std::setprecision( 3 );
            std::cout << static_cast< double >( stopwatch1.time( ) ) / stopwatch2.time( );
            std::cout << std::endl;
        }
    }
    return 0;
}
