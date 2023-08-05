/*! \file    main.cpp
    \brief   Main program of the parallel find demonstration application.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

    This program implements a parallel version of std::find and then does a benchmark test of
    its performance as compared to the the std::find in the implementation's library. This
    version explicitly manages the threads using Boost threads.
*/

// Standard C++
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <vector>

// Boost
#include <boost/thread.hpp>
#include <boost/ref.hpp>

// Application support
#include <Timer.hpp>


namespace parallel {

    // This structure encapsulates the results returned by a single thread.
    template< typename RandomAccess, typename T >
    struct FindHelper {
        bool found;          // true if this thread found the item.
        RandomAccess result; // Points at the item if found; undefined otherwise.

        FindHelper( ) : found( false ) { }

        // Call to search the indicated range for 'value'. Results left in the members above.
        void operator( )( RandomAccess first, RandomAccess last, const T *value );
    };

    // Helper function that is executed by each thread.
    template< typename RandomAccess, typename T >
    void FindHelper< RandomAccess, T >::operator( )( RandomAccess first, RandomAccess last, const T *value )
    {
        // Use the library find to search this range.
        result = std::find( first, last, *value );

        // Did we find the item?
        found = (result != last);
    }


    template< typename RandomAccess, typename T >
    RandomAccess find( RandomAccess first, RandomAccess last, const T &value )
    {
        // Assume we don't find the item.
        RandomAccess overall_result = last;

        // Don't try to use multiple threads for short sequences. Overhead is too large.
        if( last - first < 10000 ) return std::find( first, last, value );

        // Note: unsinged int implicitly converted to int.
        int thread_count = boost::thread::hardware_concurrency( );
        
        // Thread management data.
        std::vector< boost::thread * > thread_handles( thread_count );
        std::vector< FindHelper<RandomAccess, T> > results( thread_count );
        typename std::iterator_traits<RandomAccess>::difference_type
            distance = ( last - first ) / thread_count;

        // Start the worker threads.
        for( int i = 0; i < thread_count; ++i ) {
            RandomAccess thread_first = first + ( i * distance );
            RandomAccess thread_last;
            if( i == thread_count - 1)
                thread_last = last;
            else
                thread_last  = first + ( (i + 1) * distance );
            thread_handles[i] = new boost::thread( boost::ref( results[i] ), thread_first, thread_last, &value );
        }

        // Wait for the results. Wait in reverse order so that we return the first occurrence in the sequence.
        for( int i = thread_count - 1; i >= 0; --i ) {
            thread_handles[i]->join( );
            if( results[i].found ) {
                overall_result = results[i].result;
            }
            delete thread_handles[i];
        }

        return overall_result;
    }

}

int main( )
{
    spica::Timer stopwatch1, stopwatch2;
    std::vector< int > my_vector;
    
    std::cout << " Length    std  parallel  Speedup\n";
    std::cout << "=================================\n";
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
            p2 = parallel::find( my_vector.begin(), my_vector.end(), length );
        stopwatch2.stop( );

        if( p2 != my_vector.end() ) {
            std::cout << "parallel::find failed!\n";
        }
        else {
            std::cout << std::setw(7) << length             << ": ";
            std::cout << std::setw(5) << stopwatch1.time( ) << "  ";
            std::cout << std::setw(8) << stopwatch2.time( ) << "  ";
            std::cout << std::showpoint << std::setprecision( 3 );
            std::cout << std::setw(7) << static_cast< double >( stopwatch1.time( ) ) / stopwatch2.time( );
            std::cout << std::endl;
        }
    }
    return 0;
}
