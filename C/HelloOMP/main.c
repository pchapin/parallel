/*!
 *  \file   main.c
 *  \brief  OpenMP demonstration program.
 *  \author (C) Copyright 2014 by Peter C. Chapin <pchapin@vtc.edu>
 *
 *  This program exercises various OpenMP features in a silly way. It is intended to demonstrate
 *  some of the facilities of OpenMP in an environment that actually compiles and runs. Some of
 *  the examples here were taken from the book "Using OpenMP" by Barbara Chapman, Gabriele Jost,
 *  and And Ruud Van Der Pas (ISBN=978-0-262-53302-7).
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <omp.h>

void demonstrate_parallel( )
{
    // A team of threads is created here.
    #pragma omp parallel
    {
        // I/O operations might be interleaved between threads resulting in uncoordinated output.
        printf( "Parallel region executed by thread %d\n", omp_get_thread_num( ) );

        // One way to get the different threads in a team to do slightly different things.
        if( omp_get_thread_num( ) == 1 ) {
            printf( "\tThread 1 does special things.\n" );
        }
    }
    // Implicit barrier.
}


void demonstrate_parallel_for( )
{
    int i;

    // The basic parallel for construct.
    // The programmer ensures that the loop iterations do not interfere.

    const size_t size = 1000000;
    double *values = (double *)malloc( size * sizeof( double ) );

    #pragma omp parallel for
    for( i = 0; i < size; ++i ) {
        // Iterations are independent.
        values[i] = (double)i;
    }
    // Implicit barrier.
    // No iterations of following loop occur until all iterations of the previous loop complete.

    #pragma omp parallel for
    for( i = 0; i < size; ++i ) {
        // Iterations are independent.
        values[i] = 2.0 * sqrt( values[i] );
    }
    // Implicit barrier.
    // All iterations of previous loop complete before function returns.

    free( values );
}


void demonstrate_critical( )
{
    const size_t size = 1000000;
    double *values = (double *)malloc( size * sizeof( double ) );
    double sum = 0.0;
    double local_sum;
    int    TID;
    int    i;

    // Variable sharing by the threads in a team made explicit.
    #pragma omp parallel for shared(values)
    for( i = 0; i < size; ++i ) {
        values[i] = (double)i;
    }

    // Create a parallel region with sum shared and local_sum private.
    #pragma omp parallel shared(values, sum) private(TID, local_sum)
    {
        // Private variables uninitialized in parallel region (unless firstprivate used).
        TID = omp_get_thread_num( );
        local_sum = 0.0;

        // Split the loop over the team. Each thread executes only part of the loop.
        #pragma omp for
        for( i = 0; i < size; ++i ) local_sum += values[i];

        // Only one thread at a time executes the critical construct.
        #pragma omp critical
        {
            // Add thread specific results into accumulator. The I/O is not interleaved.
            sum += local_sum;
            printf( "TID = %d; local_sum = %f; sum = %f\n", TID, local_sum, sum );
        }
    }

    printf( "True answer (approx) = %f\n", ( ( (double)size - 1 ) * size ) / 2.0 );
    free( values );
}


void demonstrate_reduction( )
{
    const size_t size = 1000000;
    double *values = (double *)malloc( size * sizeof( double ) );
    double sum = 0.0;
    int    i;

    // Variable sharing by the threads in a team made explicit.
    #pragma omp parallel for shared(values)
    for( i = 0; i < size; ++i ) {
        values[i] = (double)i;
    }

    // Create a parallel region with sum shared and local_sum private.
    #pragma omp parallel for shared(values) reduction(+:sum)
    for( i = 0; i < size; ++i ) {
        sum += values[i];
    }
    printf( "Computed answer (approx) = %f\n", sum );
    printf( "True answer (approx) = %f\n", ( ( (double)size - 1 ) * size ) / 2.0 );
    free( values );
}


void demonstrate_ordered( )
{
    const size_t size = 10;
    double values[10];
    int    i;

    // Loop not parallelized (only 10 passes).
    for( i = 0; i < size; ++i ) {
        values[i] = (double)i;
    }

    // Loop parallelized for demonstration purposes. Note that 'ordered' clause is required.
    #pragma omp parallel for ordered
    for( i = 0; i < size; ++i ) {
        values[i] = sqrt( values[i] );
        #pragma omp ordered
        {
            printf( "values[%d] = %f\n", i, values[i] );
        }
    }
}


static void f( )
{
    printf( "Executing function f( )\n" );
}

static void g( )
{
    printf( "Executing function g( )\n" );
}

void demonstrate_sections( )
{
    // Create a parallel region with different sections.
    #pragma omp parallel sections
    {
        // Execute the sections in parallel. Each section can do different things.
        #pragma omp section
        f( );

        #pragma omp section
        g( );
    }
}


int main( )
{
    int done = 0;
    int choice;

    do {
        // Display menu.
        printf(
            "\n"
            "0) Quit\n"
            "1) Parallel construct\n"
            "2) Parallel for construct\n"
            "3) Critical construct\n"
            "4) Reduction clause\n"
            "5) Ordered construct\n"
            "6) Sections construct\n" );

        printf( "\n" );
        printf( "Selection: " );
        scanf( "%d", &choice );
        printf( "\n" );

        // Handle menu selection.
        switch( choice ) {
        case 0: done = 1; break;
        case 1: demonstrate_parallel( );     break;
        case 2: demonstrate_parallel_for( ); break;
        case 3: demonstrate_critical( );     break;
        case 4: demonstrate_reduction( );    break;
        case 5: demonstrate_ordered( );      break;
        case 6: demonstrate_sections( );     break;
        }
    } while( !done );

    return EXIT_SUCCESS;
}
