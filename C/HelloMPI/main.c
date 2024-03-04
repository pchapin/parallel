/*!
 * \file    HelloMPI.c
 * \brief   Sample MPI program that sends and receives some basic messages.
 * \author  Peter Chapin <peter.chapin@vermontstate.edu>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define BUFFER_SIZE 128

int main( int argc, char **argv )
{
    int number_of_processes;
    int my_rank;
    int source;
    int destination;
    int tag_value;
    MPI_Status status;
    char buffer[BUFFER_SIZE];

    MPI_Init( &argc, &argv );
    MPI_Comm_size( MPI_COMM_WORLD, &number_of_processes );
    MPI_Comm_rank( MPI_COMM_WORLD, &my_rank );

    if( my_rank == 0 ) {
        // I'm the main node...
        for( source = 1; source < number_of_processes; ++source ) {
            tag_value = 42;
            MPI_Recv( buffer, BUFFER_SIZE, MPI_CHAR, source, tag_value, MPI_COMM_WORLD, &status );
            printf( "%s\n", buffer );
        }
    }
    else {
        // I'm a worker bee...
        sprintf( buffer, "Hello from process %d.", my_rank );
        destination = 0;
        tag_value   = 42;
        MPI_Send( buffer, (int)strlen( buffer ) + 1, MPI_CHAR, destination, tag_value, MPI_COMM_WORLD );
    }
    MPI_Finalize( );
    return EXIT_SUCCESS;
}
