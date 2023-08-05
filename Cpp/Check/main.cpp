/*! \file    main.cpp
    \brief   Main program of the unit test program.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#include <iostream>
#include <iomanip>
#include <boost/ref.hpp>
#include <boost/thread.hpp>
#include <BoundedBuffer.hpp>
#include <ThreadPool.hpp>

const int Item_Count = 100000;
spica::BoundedBuffer<int> shared_buffer;

namespace {

    // The following functions are used by the bounded buffer test.
    // ------------------------------------------------------------

    void producer( )
    {
        for( int i = 0; i < Item_Count; ++i ) {
            shared_buffer.push( i );
        }
    }

    void consumer( )
    {
        int result;

        for( int i = 0; i < Item_Count; ++i ) {
            result = shared_buffer.pop( );
            if( result != i ) std::cout << "Unexpected item consumed: " << i << std::endl;
        }
    }

    // The following functions are used by the ThreadPool test.
    // --------------------------------------------------------

    boost::mutex console_lock;

    void work_0( )
    {
        boost::lock_guard< boost::mutex > guard( console_lock );
        std::cout << "Hello, World!" << std::endl;
    }

    void work_1( const char *message )
    {
        boost::lock_guard< boost::mutex > guard( console_lock );
        std::cout << message << std::endl;
    }

    void work_2( const char *message, int count )
    {
        for( int i = 0; i < count; ++i ) {
            boost::lock_guard< boost::mutex > guard( console_lock );
            std::cout << std::setw( 2 ) << i << ": " << message << std::endl;
        }
    }

    struct SumHelper {
        int partial_sum;
        void operator( )( const int *first, const int *last );
    };

    void SumHelper::operator( )( const int *first, const int *last )
    {
        partial_sum = 0;
        while( first != last ) {
            partial_sum += *first;
            ++first;
        }
    }


    boost::mutex torture_lock;
    int torture_count = 0;

    void pool_torturer( )
    {
        boost::lock_guard<boost::mutex> guard( torture_lock );
        torture_count++;
    }

}


void bounded_buffer_test( )
{
    std::cout << "Running the bounded buffer test..." << std::endl;

    // Create the two threads...
    boost::thread producer_thread( producer );
    boost::thread consumer_thread( consumer );

    // ... and wait for them to end.
    producer_thread.join( );
    consumer_thread.join( );

    std::cout << "Bounded buffer test complete.\n" << std::endl;
}


void threadpool_test( )
{
    std::cout << "Running the ThreadPool test..." << std::endl;

    // Create a thread pool.
    spica::ThreadPool my_pool;

    std::cout << "Number of threads in the pool = " << my_pool.count( ) << std::endl;

    const char *m1 = "Hello 1";
    const char *m2 = "Hello 2";

    std::cout << "Part #0:" << std::endl;
    spica::ThreadPool::threadid_t ID1 = my_pool.start_work( work_0 );
    spica::ThreadPool::threadid_t ID2 = my_pool.start_work( work_0 );
    my_pool.work_result( ID1 );
    my_pool.work_result( ID2 );

    std::cout << "Part #1:" << std::endl;
    ID1 = my_pool.start_work( work_1, m1 );
    ID2 = my_pool.start_work( work_1, m2 );
    my_pool.work_result( ID1 );
    my_pool.work_result( ID2 );

    std::cout << "Part #2:" << std::endl;
    ID1 = my_pool.start_work( work_2, m1, 10 );
    ID2 = my_pool.start_work( work_2, m2, 10 );
    my_pool.work_result( ID1 );
    my_pool.work_result( ID2 );

#ifdef NEVER
    std::cout << "Returning results:" << std::endl;
    int data_array[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    SumHelper results[2];
    ID1 = my_pool.start_work( boost::ref( results[0] ), data_array + 0, data_array + 4 );
    ID2 = my_pool.start_work( boost::ref( results[1] ), data_array + 4, data_array + 8 );
    my_pool.work_result( ID1 );
    my_pool.work_result( ID2 );
    if( results[0].partial_sum + results[1].partial_sum != 36 )
        std::cout << "Returning results FAILED!" << std::endl;
#endif

    std::cout << "Torture test:" << std::endl;
    std::vector<spica::ThreadPool::threadid_t> IDs( my_pool.count( ) );
    for( int i = 1; i <= 1000000; ++i ) {
        for( int thread_counter = 0; thread_counter < my_pool.count( ); ++thread_counter ) {
            IDs[thread_counter] = my_pool.start_work( pool_torturer );
        }
        for( int thread_counter = 0; thread_counter < my_pool.count( ); ++thread_counter ) {
            my_pool.work_result( IDs[thread_counter] );
        }
    }
    if( torture_count != 1000000 * my_pool.count( ) ) {
        std::cout << "  FAILED!" << std::endl;
    }
    else {
        std::cout << "  Passed" << std::endl;
    }

    std::cout << "ThreadPool test complete.\n" << std::endl;
}


int main( )
{
    bounded_buffer_test( );
    threadpool_test( );
    return 0;
}