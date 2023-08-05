/*! \file    sudoku.hpp
    \brief   Interface to a Sudoku board abstraction.
    \author  Peter C. Chapin <PChapin@vtc.vsc.edu>

*/

#ifndef SUDOKU_HPP
#define SUDOKU_HPP

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "Timer.hpp"

class SolutionManager;

//! A representation of a Sudoku puzzle.
class SudokuBoard {

    friend void solver( SudokuBoard &current, int next_row, int next_col );

public:

    //! Exceptions of this class are thrown when SudokuBoard methods encounter an error.
    class Error : public std::runtime_error {
    public:
        Error( const std::string &message ) : std::runtime_error( message ) { }
    };

    SudokuBoard( SolutionManager *my_manager, int size = 9 );

    //! Reads a board from a file.
    void load( const std::string &file_name );

    //! Finds and displays all solutions.
    void solve( );

    //! Displays the board on the indicated output stream.
    void display( std::ostream &os );

private:
    int n;           //!< The length of one side of the board.
    int root_n;      //!< The square root of n.
    int filled;      //!< The number of occupied positions. Used during solving.
    std::vector<int>  board;    //!< Current board. Zeros mean no value.
    std::vector<bool> fixed;    //!< Indicates which positions are "fixed" and not in play.
    SolutionManager  *manager;  //!< Points at the object managing the solutions for this board.

    //! Erases the board to an empty state.
    void erase_board( );

    //! Return true if the board satisfies Sudoku rules; false otherwise.
    bool valid_board( );

    //! Return true if the board satisfies Sudoku rules; false otherwise.
    /*!
     *  This version only checks the row, column, and region that includes the indicated
     *  position. It can be used if the board was known to be valid before the value at the
     *  indicated position was placed.
     */
    bool incremental_valid_board( int row, int col );
};


//! Manages sudoku solutions (tracks how many have been found, etc).
class SolutionManager {
public:
    SolutionManager( ) : solutions( 0 )
        { }

    //! Increments the count of solutions found.
    void new_solution( );

    //! Return the total number of solutions found. Only valid after solve( ) has returned.
    long long solution_count( )
        { return solutions; }

    void reset_count( )
        { solutions = 0; }

    //! Start timing the solution process.
    void start_timer( )
        { stopwatch.start( ); }

    //! Stop timing the solution process.
    void stop_timer( )
        { stopwatch.stop( ); }

private:
    long long    solutions;  //!< The number of solutions found.
    spica::Timer stopwatch;  //!< Used to time the solving process.
};

#endif
