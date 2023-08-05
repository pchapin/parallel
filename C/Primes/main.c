
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int is_prime( unsigned long long n )
{
    if( n <= 1 ) return 0;      // Error check for values that are too small.
    if( n % 2 == 0 ) return 0;  // Even numbers are not prime.

    unsigned long long upper_bound = n;
    for( unsigned long long i = 3; i < upper_bound; i += 2 ) {
        upper_bound = n / i;
        if( n % i == 0 ) return 0;
    }
    return 1;
}


int main( int argc, char **argv )
{
    int number_of_processes;
    int my_rank;
    unsigned long upper_bound;

    if( argc != 2 ) {
        fprintf( stderr, "Usage: %s upper_bound.\n", argv[0] );
        return EXIT_FAILURE;
    }
    upper_bound = atol( argv[1] );
    if( upper_bound < 1 ) {
        fprintf( stderr, "Upper bound of %ld is invalid.\n", upper_bound );
        return EXIT_FAILURE;
    }

    MPI_Init( &argc, &argv );
    MPI_Comm_size( MPI_COMM_WORLD, &number_of_processes );
    MPI_Comm_rank( MPI_COMM_WORLD, &my_rank );

    if( my_rank == 0 ) {
        printf( "Using upper bound = %ld\n", upper_bound );
    }

    int number_of_values = (upper_bound - 1) / number_of_processes;
    // TODO: The last node might need to take on a few extra values if the above doesn't divide evenly.

    unsigned long long *problem_description =
        (unsigned long long *)malloc( 2 * number_of_processes * sizeof( unsigned long long ) );
    // TODO: Check that the allocation above is successful.
    unsigned long long work_unit[2];

    for( int i = 0; i < number_of_processes; ++i ) {
        problem_description[2*i] = number_of_values*i;
        problem_description[2*i + 1] = number_of_values;
    }

    MPI_Scatter
        ( problem_description, 2, MPI_UNSIGNED_LONG, work_unit, 2, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD );

    unsigned long long count = 0;
    for( unsigned long long i = work_unit[0]; i < work_unit[0] + work_unit[1]; ++i ) {
        if( is_prime(i) ) count++;
    }

    printf( "partial pi(n) = %lld\n", count );

    free( problem_description );

    MPI_Finalize( );
    return EXIT_SUCCESS;
}
