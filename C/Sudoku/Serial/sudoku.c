/*! \file    sudoku.c
 *  \brief   Implementation of a Sudoku board abstraction.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "sudoku.h"

#define PRIVATE static
#define PUBLIC

//! Return true if the give size is acceptable for a Sudoku board.
PRIVATE int valid_size( int size, int *root_size )
{
    static const int valid_sizes[] = { 4, 9, 16 };
    static const int valid_root_sizes[] = { 2, 3, 4 };
    static const int number_of_sizes = sizeof( valid_sizes ) / sizeof( int );

    for( int i = 0; i < number_of_sizes; ++i ) {
        if( valid_sizes[i] == size ) {
            *root_size = valid_root_sizes[i];
            return true;
        }
    }
    return false;
}


//! Scan a board from a given position looking for the next open position.
/*!
 * The values of *next_row and *next_col are initialized to the last occupied position. This
 * function assumes all positions above and to the left of that position are also occupied. It
 * scans below and to the right for an unoccupied (non "fixed") position and writes the
 * coordinates of that position into *next_row and *next_col. This function assumes such a
 * position exists. If one does not exist, the board has already been solved and we shouldn't be
 * in this function.
 */
PRIVATE void find_next( const int *fixed, int n, int *next_row, int *next_col )
{
    // Search to the right of the last occupied position.
    for( int j = *next_col + 1; j < n; ++j ) {
        if( fixed[*next_row * n + j] ) continue;
        *next_col = j;
        return;
    }

    // Search remaining rows.
    for( int i = *next_row + 1; i < n; ++i ) {
        for( int j = 0; j < n; ++j ) {
            if( fixed[i * n + j] ) continue;
            *next_row = i;
            *next_col = j;
            return;
        }
    }

    // Should never get here.
    assert( 0 == 1 );
}


// SudokuBoard Private Methods
// ---------------------------

//! Return true if the board satisfies Sudoku rules; false otherwise.
PRIVATE int SudokuBoard_valid_board( SudokuBoard *object )
{
    int *flags = (int *)malloc( object->n * sizeof( int ) );

    // Check rows.
    for( int row = 0; row < object->n; ++row ) {

        memset( flags, 0, object->n * sizeof( int ) );
        for( int col = 0; col < object->n; ++col ) {
            int value = object->board[ row * object->n + col ];
            if( value == 0 ) continue;
            if( flags[value - 1] == 1 ) {
                free( flags );
                return false;  // We've seen this value before. Invalid.
            }
            flags[value - 1] = 1;
        }
    }

    // Check cols.
    for( int col = 0; col < object->n; ++col ) {

        memset( flags, 0, object->n * sizeof( int ) );
        for( int row = 0; row < object->n; ++row ) {
            int value = object->board[ row * object->n + col ];
            if( value == 0 ) continue;
            if( flags[value - 1] == 1 ) {
                free( flags );
                return false;  // We've seen this value before. Invalid.
            }
            flags[value - 1] = 1;
        }
    }

    // Check regions.
    for( int region_number = 0; region_number < object->n; ++region_number ) {
        int start_row = object->root_n * ( region_number / object->root_n );
        int start_col = object->root_n * ( region_number % object->root_n );

        memset( flags, 0 , object->n * sizeof( int ) );
        for( int i = start_row; i < start_row + object->root_n; ++i ) {
            for( int j = start_col; j < start_col + object->root_n; ++j ) {
                int value = object->board[ i * object->n + j ];
                if( value == 0 ) continue;
                if( flags[value - 1] == 1 ) {
                    free( flags );
                    return false;  // We've seen this value before. Invalid.
                }
                flags[value - 1] = 1;
            }
        }
    }

    // If we get here, everything is ok.
    free( flags );
    return true;
}


//! Return true if the board satisfies Sudoku rules; false otherwise.
/*!
 *  This version only checks the row, column, and region that includes the indicated
 *  position. It can be used if the board was known to be valid before the value at the
 *  indicated position was placed.
 */
PRIVATE int SudokuBoard_incremental_valid_board( SudokuBoard *object, int row, int col )
{
    int flags[9];

    // Check row.
    memset( flags, 0, object->n * sizeof( int ) );
    for( int temp_col = 0; temp_col < object->n; ++temp_col ) {
        int value = object->board[ row * object->n + temp_col ];
        if( value == 0 ) continue;
        if( flags[value - 1] == 1 ) return false;  // We've seen this value before. Invalid.
        flags[value - 1] = 1;
    }

    // Check col.
    memset( flags, 0, object->n * sizeof( int ) );
    for( int temp_row = 0; temp_row < object->n; ++temp_row ) {
        int value = object->board[ temp_row * object->n + col ];
        if( value == 0 ) continue;
        if( flags[value - 1] == 1 ) return false;  // We've seen this value before. Invalid.
        flags[value - 1] = 1;
    }

    // Check region.
    int start_row = object->root_n * ( row / object->root_n );
    int start_col = object->root_n * ( col / object->root_n );

    memset( flags, 0 , object->n * sizeof( int ) );
    for( int i = start_row; i < start_row + object->root_n; ++i ) {
        for( int j = start_col; j < start_col + object->root_n; ++j ) {
            int value = object->board[ i * object->n + j ];
            if( value == 0 ) continue;
            if( flags[value - 1] == 1 ) return false;  // We've seen this value before. Invalid.
            flags[value - 1] = 1;
        }
    }

    // If we get here, everything is ok.
    return true;
}


//! Finds all Sudoku solutions starting at the "current position."
/*!
 * This function assumes that all board positions above and to the left of the current position
 * have already been filled in. It tries all possible values at the current position, printing
 * solutions as it goes.
 */
PRIVATE void SudokuBoard_solver( SudokuBoard *puzzle, const int current_row, const int current_col )
{
    int next_row;
    int next_col;

    // Try every possible value at the current position.
    puzzle->filled++;
    for( int i = 1; i <= puzzle->n; ++i ) {
        puzzle->board[ current_row * puzzle->n + current_col ] = i;
        if( SudokuBoard_incremental_valid_board( puzzle, current_row, current_col ) ) {

            // Did we solve it?
            if( puzzle->filled == puzzle->n * puzzle->n ) {
                SolutionManager_new_solution( puzzle->manager );
                // SudokuBoard_display( puzzle, stdout );
            }
            // Otherwise, call ourselves to handle the rest of the board.
            else {
                next_row = current_row;
                next_col = current_col;
                find_next( puzzle->fixed, puzzle->n, &next_row, &next_col );
                SudokuBoard_solver( puzzle, next_row, next_col );
            }
        }
    }

    // Remove this placement before returning to the previous level.
    puzzle->filled--;
    puzzle->board[ current_row * puzzle->n + current_col ] = 0;
}


//! Erases the board to an empty state.
PRIVATE void SudokuBoard_erase_board( SudokuBoard *object )
{
    for( int i = 0; i < object->n * object->n; ++i ) {
        object->fixed[i] = false;
        object->board[i] = 0;
    }
    object->filled = 0;
}



// SudokuBoard Public Methods
// --------------------------

PUBLIC void SudokuBoard_construct( SudokuBoard *object, SolutionManager *my_manager, int size )
{
    assert( my_manager != NULL );

    if( !valid_size( size, &object->root_n ) ) {
        // TODO: What should happen if we are given an invalid size!
    }

    object->n       = size;
    object->filled  = 0;
    object->manager = my_manager;
    object->board   = (int *)malloc( object->n * object->n * sizeof( int ) );
    object->fixed   = (int *)malloc( object->n * object->n * sizeof( int ) );
}


PUBLIC void SudokuBoard_destroy( SudokuBoard *object )
{
    free( object->board );
    free( object->fixed );
}


PUBLIC void SudokuBoard_load( SudokuBoard *object, const char *file_name )
{
    int value;

    FILE *input;

    if( ( input = fopen( file_name, "r" ) ) == NULL ) {
        abort( );  // TODO: Come up with something better!
    }

    object->filled = 0;
    SolutionManager_reset_count( object-> manager );

    // Try to read all board values.
    for( int i = 0; i < object->n * object->n; ++i ) {
        fscanf( input, "%d", &value );
        if( feof( input ) )
            abort( );  // TODO: Come up with something better!

        if( value == 0 ) {
            object->fixed[i] = false;
            object->board[i] = value;
        }
        else {
            if( value < 1 || value > object->n ) {
                SudokuBoard_erase_board( object );
                abort( );  // TODO: Come up with something better!
            }
            object->fixed[i] = true;
            object->board[i] = value;
            ++object->filled;
        }
    }

    if ( !SudokuBoard_valid_board( object ) ) {
        SudokuBoard_erase_board( object );
        abort( );  // TODO: Come up with something better!
    }

    fclose( input );
}


PUBLIC void SudokuBoard_solve( SudokuBoard *object )
{
    SolutionManager_reset_count( object->manager );
    SolutionManager_start_timer( object->manager );

    // Is it solved already (fully fixed)? If not find the first non-fixed position...
    if( object->filled == object->n * object->n ) {
        SolutionManager_new_solution( object->manager );
        // SudokuBoard_display( object, stdout );
    }
    else {
        int next_row;
        int next_col;

        // Look for the first non-fixed position (there must be at least one).
        for( int i = 0; i < object->n; ++i ) {
            for( int j = 0; j < object->n; ++j ) {
                if( !object->fixed[i * object->n + j] ) {
                    next_row = i;
                    next_col = j;
                    goto execute_solve;
                }
            }
        }
        // The loops above should only exit via the goto.
        assert( false );

execute_solve:
        SudokuBoard_solver( object, next_row, next_col );
    }

    SolutionManager_stop_timer( object->manager );
}


PUBLIC void SudokuBoard_display( SudokuBoard *object, FILE *output )
{
    fprintf( output, "SudokuBoard\n" );
    fprintf( output, "-----------\n" );
    for( int row = 0; row < object->n; ++row ) {
        for( int col = 0; col < object->n; ++col ) {
            fprintf( output, "  %d", object->board[row * object->n + col] );
        }
        fprintf( output, "\n\n" );
    }
}


// SolutionManager Public Methods
// ------------------------------

PUBLIC void SolutionManager_construct( SolutionManager *object )
{
    object->solutions = 0;
    Timer_initialize( &object->stopwatch );
}


PUBLIC void SolutionManager_new_solution( SolutionManager *object )
{
    object->solutions++;
    if( object->solutions % 100000 == 0) {
        double seconds_elapsed = (double)( Timer_time( &object->stopwatch ) ) / 1000.0;
        printf( "\r%lli solutions found (%.1f sols/s)",
                object->solutions, (double)object->solutions / seconds_elapsed );
        fflush( stdout );
        // if( object->solutions == 200000 ) exit( 0 );
    }
}
