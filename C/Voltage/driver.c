/*! \file    driver.c
 *  \brief   A simple program to call the appropriate main.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

extern int main_0( void );  // The serial version.
extern int main_1( void );  // The parallel version using pthreads.
extern int main_2( void );  // The parallel version using MPI + OpenMP.

int main( void )
{
    // TODO: I should be able to select which flavor to run based on a command line argument.
    return main_1( );
}
