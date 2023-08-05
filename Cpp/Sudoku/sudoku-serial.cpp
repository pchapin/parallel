/*! \file    sudoku.cpp
    \brief   Implementation of a Sudoku board abstraction.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#include <cassert>
#include <fstream>
#include <iomanip>

#include <boost/scoped_array.hpp>
#include "sudoku.hpp"

namespace {

    //! Return true if the give size is acceptable for a Sudoku board.
    bool valid_size( int size, int &root_size )
    {
        static const int valid_sizes[] = { 4, 9, 16 };
        static const int valid_root_sizes[] = { 2, 3, 4 };
        static const int number_of_sizes = sizeof( valid_sizes ) / sizeof( int );

        // TODO: Check for overflow in the case where size * size is too large.
        for( int i = 0; i < number_of_sizes; ++i ) {
            if( valid_sizes[i] == size ) {
                root_size = valid_root_sizes[i];
                return true;
            }
        }
        return false;
    }


    //! Scan a board from a given position looking for the next open position.
    /*!
     * The values of next_row and next_col are initialized to the last occupied position. This
     * function assumes all positions above and to the left of that position are also occupied.
     * It scans below and to the right for an unoccupied (non "fixed") position and writes the
     * coordinates of that position into next_row and next_col. This function assumes such a
     * position exists. If one does not exist, the board has already been solved and we
     * shouldn't be in this function.
     */
    void find_next( const std::vector<bool> &fixed, int n, int &next_row, int &next_col )
    {
        // Search to the right of the last occupied position.
        for( int j = next_col + 1; j < n; ++j ) {
            if( fixed[next_row * n + j] ) continue;
            next_col = j;
            return;
        }

        // Search remaining rows.
        for( int i = next_row + 1; i < n; ++i ) {
            for( int j = 0; j < n; ++j ) {
                if( fixed[i * n + j] ) continue;
                next_row = i;
                next_col = j;
                return;
            }
        }

        // Should never get here.
        assert(0 == 1);
    }

}


//! Finds all Sudoku solutions starting at the "next position."
/*!
 * This function assumes that all board positions above and to the left of the next position
 * have already been filled in. It tries all possible values at the next position, printing
 * solutions as it goes.
 */
void solver( SudokuBoard &puzzle, const int current_row, const int current_col )
{
    int next_row;
    int next_col;

    // Try every possible value at the current position.
    puzzle.filled++;
    for( int i = 1; i <= puzzle.n; ++i ) {
        puzzle.board[ current_row * puzzle.n + current_col ] = i;
        if( puzzle.incremental_valid_board( current_row, current_col ) ) {

            // Did we solve it? If not, call ourselves to handle the rest of the board.
            if( puzzle.filled == puzzle.n * puzzle.n ) {
                puzzle.manager->new_solution( );
                // puzzle.display( std::cout );
            }
            else {
                next_row = current_row;
                next_col = current_col;
                find_next( puzzle.fixed, puzzle.n, next_row, next_col );
                solver( puzzle, next_row, next_col );
            }
        }
    }

    // Remove this placement before returning to the previous level.
    puzzle.filled--;
    puzzle.board[ current_row * puzzle.n + current_col ] = 0;
}


SudokuBoard::SudokuBoard( SolutionManager *my_manager, int size )
{
    if( my_manager == NULL )
        throw Error( "NULL SolutionManager given to SudokuBoard constructor" );
    if( !valid_size( size, root_n ) )
        throw Error( "Invalid size given to SudokuBoard constructor" );

    n       = size;
    filled  = 0;
    manager = my_manager;
    board.resize( n * n );
    fixed.resize( n * n );
}


void SudokuBoard::load( const std::string &file_name )
{
    int value;

    std::ifstream input( file_name.c_str( ) );
    if( !input )
        throw Error( "Unable to open file containing Sudoku board" );

    filled = 0;
    manager->reset_count( );

    // Try to read all board values.
    for( int i = 0; i < n * n; ++i ) {
        input >> value;
        if( input.eof( ) )
            throw Error( "Unexpected end-of-file while reading Sudoko board" );

        if( value == 0 ) {
            fixed[i] = false;
            board[i] = value;
        }
        else {
            if( value < 1 || value > n ) {
                erase_board( );
                throw Error( "Bad board value in Sudoku board input file" );
            }
            fixed[i] = true;
            board[i] = value;
            ++filled;
        }
    }

    if ( !valid_board( ) ) {
        erase_board( );
        throw Error( "Invalid Sudoku board in input file" );
    }
}


void SudokuBoard::solve( )
{
    manager->reset_count( );
    manager->start_timer( );

    // Is it solved already (fully fixed)? If not find the first non-fixed position...
    if( filled == n * n ) {
        manager->new_solution( );
        // display( std::cout );
    }
    else {
        int next_row;
        int next_col;

        // Look for the first non-fixed position (there must be at least one).
        for( int i = 0; i < n; ++i ) {
            for( int j = 0; j < n; ++j ) {
                if( !fixed[i * n + j] ) {
                    next_row = i;
                    next_col = j;
                    goto execute_solve;
                }
            }
        }
        // The loops above should only exit via the goto.
        assert( 0 == 1 );

execute_solve:
        solver( *this, next_row, next_col );
    }

    manager->stop_timer( );
}


void SudokuBoard::display( std::ostream &os )
{
    os << "SudokuBoard\n";
    os << "-----------\n";
    for( int row = 0; row < n; ++row ) {
        for( int col = 0; col < n; ++col ) {
            os << std::setw( 2 ) << "  " << board[ row * n + col ];
        }
        os << "\n\n";
    }
}


void SudokuBoard::erase_board( )
{
    for( int i = 0; i < n * n; ++i ) {
        fixed[i] = false;
        board[i] = 0;
    }
    filled = 0;
}


bool SudokuBoard::valid_board( )
{
    boost::scoped_array<int> flags( new int[n] );

    // Check rows.
    for( int row = 0; row < n; ++row ) {

        std::memset( flags.get( ), 0, n * sizeof( int ) );
        for( int col = 0; col < n; ++col ) {
            int value = board[ row * n + col ];
            if( value == 0 ) continue;
            if( flags[value - 1] == 1 ) return false;  // We've seen this value before. Invalid.
            flags[value - 1] = 1;
        }
    }

    // Check cols.
    for( int col = 0; col < n; ++col ) {

        std::memset( flags.get( ), 0, n * sizeof( int ) );
        for( int row = 0; row < n; ++row ) {
            int value = board[ row * n + col ];
            if( value == 0 ) continue;
            if( flags[value - 1] == 1 ) return false;  // We've seen this value before. Invalid.
            flags[value - 1] = 1;
        }
    }

    // Check regions.
    for( int region_number = 0; region_number < n; ++region_number ) {
        int start_row = root_n * ( region_number / root_n );
        int start_col = root_n * ( region_number % root_n );
        
        std::memset( flags.get( ), 0 , n * sizeof( int ) );
        for( int i = start_row; i < start_row + root_n; ++i ) {
            for( int j = start_col; j < start_col + root_n; ++j ) {
                int value = board[ i * n + j ];
                if( value == 0 ) continue;
                if( flags[value - 1] == 1 ) return false;  // We've seen this value before. Invalid.
                flags[value - 1] = 1;
            }
        }
    }

    // If we get here, everything is ok.
    return true;
}


bool SudokuBoard::incremental_valid_board( int row, int col )
{
    // boost::scoped_array<int> flags( new int[n] );
    int flags[9];

    // Check row.
    std::memset( flags /* .get( ) */, 0, n * sizeof( int ) );
    for( int temp_col = 0; temp_col < n; ++temp_col ) {
        int value = board[ row * n + temp_col ];
        if( value == 0 ) continue;
        if( flags[value - 1] == 1 ) return false;  // We've seen this value before. Invalid.
        flags[value - 1] = 1;
    }

    // Check col.
    std::memset( flags /* .get( ) */, 0, n * sizeof( int ) );
    for( int temp_row = 0; temp_row < n; ++temp_row ) {
        int value = board[ temp_row * n + col ];
        if( value == 0 ) continue;
        if( flags[value - 1] == 1 ) return false;  // We've seen this value before. Invalid.
        flags[value - 1] = 1;
    }

    // Check region.
    int start_row = root_n * ( row / root_n );
    int start_col = root_n * ( col / root_n );
        
    std::memset( flags /* .get( ) */, 0 , n * sizeof( int ) );
    for( int i = start_row; i < start_row + root_n; ++i ) {
        for( int j = start_col; j < start_col + root_n; ++j ) {
            int value = board[ i * n + j ];
            if( value == 0 ) continue;
            if( flags[value - 1] == 1 ) return false;  // We've seen this value before. Invalid.
            flags[value - 1] = 1;
        }
    }

    // If we get here, everything is ok.
    return true;
}


void SolutionManager::new_solution( )
{
    solutions++;
    if( solutions % 100000 == 0) {
        double seconds_elapsed = static_cast<double>( stopwatch.time( ) ) / 1000.0;
        std::cout << "\r" << solutions << " solutions found (";
        std::cout << std::fixed << std::setprecision( 0 );
        std::cout << solutions / seconds_elapsed << " sols/s)" << std::flush;
        // if( solutions == 200000 ) std::exit( 0 );
    }
}