/*! \file    main.cpp
    \brief   Main program of parallel QuickSort demonstration etude.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include "QuickSort.hpp"
#include "ThreadPool.hpp"
#include "Timer.hpp"

spica::ThreadPool master_pool;

int *initialize_workspace( int count )
{
    int *workspace = new int[count];
    for( int i = 0; i < count; ++i ) {
        workspace[i] = i;
    }

    // This might take a while for large workspaces.
    std::random_shuffle( workspace, workspace + count );
    return workspace;
}

void check_order( int *first, int *last )
{
    int expected = 0;
    while( first != last ) {
        if( *first != expected ) {
            std::cerr << "\nUnexpected value in sorted sequence!\n";
            return;
        }
        ++expected;
        ++first;
    }
}

int main( )
{
    spica::Timer stopwatch;
    int *workspace;

    typedef void (*test_function_t)( int *, int *, std::less< int > );

    test_function_t test_functions[] = {
        std::sort< int *, std::less< int > >,
        seq::sort< int *, std::less< int > >,
        par::sort< int *, std::less< int > >
    };
    const int test_count = sizeof( test_functions ) / sizeof( test_function_t );

    std::cout << "     size    std    seq    par\n";
    std::cout << "------------------------------\n";
    for( int size = 5000000; size <= 15000000; size += 1000000 ) {
        std::cout << std::setw(9) << size;
        for( int i = 0; i < test_count; ++i ) {
            workspace = initialize_workspace( size );
            stopwatch.reset( );
            stopwatch.start( );
            test_functions[i]( workspace, workspace + size, std::less< int >( ) );
            stopwatch.stop( );
            check_order( workspace, workspace + size );
            std::cout << "  " << std::setw(5) << stopwatch.time( );
            delete [] workspace;
        }
        std::cout << "\n";
    }
    return EXIT_SUCCESS;
}