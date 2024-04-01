/*! \file    driver.c
 *  \brief   A simple program to call the appropriate main.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int main_0( void );  // The serial version.
extern int main_1( void );  // The parallel version using pthreads (and barriers).
extern int main_2( int argc, char *argv[] );  // The parallel version using OpenMP + MPI.
extern int main_3( void );  // The parallel version using CUDA.

int main( int argc, char *argv[]  )
{
    if( argc != 2 || strlen( argv[1] ) != 2 || argv[1][0] != '-' || !isdigit( argv[1][1] ) ) {
        fprintf( stderr, "Usage: %s (-0 | -1 | -2 | -3)\n"
                         "  Where -0 selects the serial version.\n"
                         "        -1 selects the parallel version using pthreads directly.\n"
                         "        -2 selects the parallel version using OpenMP + MPI.\n"
                         "        -3 selects the parallel version using CUDA.\n", argv[0] );
        return EXIT_FAILURE;
    }
    
    int option = atoi( &argv[1][1] );
    switch( option ) {
    case 0: return main_0( );
    case 1: return main_1( );
    case 2: return main_2( argc, argv );
    case 3: return main_3( );
    default:
        fprintf( stderr, "Unknown option: -%d\n", option );
        return EXIT_FAILURE;
    }
}
