/*! \file    sudoku.h
 *  \brief   Interface to a Sudoku board abstraction.
 *  \author  Peter Chapin <spicacality@kelseymountain.org>
 *
 * This is the parallel version.
 */

#ifndef SUDOKU_H
#define SUDOKU_H

#include <stdio.h>
#include <pthread.h>
#include "Timer.h"

#define true  1
#define false 0

// ===========
// SudokuBoard
// ===========

struct SolutionManager;

//! A representation of a Sudoku puzzle.
typedef struct SudokuBoard {
    int  n;           //!< The length of one side of the board.
    int  root_n;      //!< The square root of n.
    int  filled;      //!< The number of occupied positions. Used during solving.
    int *board;       //!< Current board. Zeros mean no value.
    int *fixed;       //!< Indicates which positions are "fixed" and not in play (booleans).
    struct SolutionManager *manager;  //!< Points at the object managing the solutions for this board.
} SudokuBoard;


// Public Methods
// --------------

//! Initializes a board.
void SudokuBoard_construct( SudokuBoard *object, struct SolutionManager *my_manager, int size );

//! Cleans up a board.
void SudokuBoard_destroy( SudokuBoard *object );

//! Reads a board from a file.
void SudokuBoard_load( SudokuBoard *object, const char *file_name );

//! Finds and displays all solutions.
void SudokuBoard_solve( SudokuBoard *object );

//! Displays the board on the indicated output stream.
void SudokuBoard_display( SudokuBoard *object, FILE *output );


// Private Methods TODO: Move to the .c file.
// ---------------

struct SolverArguments {
    SudokuBoard *puzzle;
    int current_row;
    int current_col;
    int available_threads;
};

//! Recursive helper function that does the solving. This function is called by multiple threads.
void *SudokuBoard_solver( void *args );

//! Erases the board to an empty state.
void SudokuBoard_erase_board( SudokuBoard *object );

//! Return true if the board satisfies Sudoku rules; false otherwise.
int SudokuBoard_valid_board( SudokuBoard *object );

//! Return true if the board satisfies Sudoku rules; false otherwise.
/*!
 *  This version only checks the row, column, and region that includes the indicated
 *  position. It can be used if the board was known to be valid before the value at the
 *  indicated position was placed.
 */
int SudokuBoard_incremental_valid_board( SudokuBoard *object, int row, int col );


// ===============
// SolutionManager
// ===============

//! Manages sudoku solutions (tracks how many have been found, etc).
typedef struct SolutionManager {
    long long solutions;   //!< The number of solutions found.
    Timer     stopwatch;   //!< Used to time the solving process.
    pthread_mutex_t lock;  //!< Used when updating solution information from multiple threads.
} SolutionManager;


// Public Methods
// --------------


//! Initializes a solution manager.
void SolutionManager_construct( SolutionManager *object );

//! Clean up a solution manager.
inline void SolutionManager_destroy( SolutionManager *object )
    { pthread_mutex_destroy( &object->lock ); }

//! Increments the count of solutions found.
void SolutionManager_new_solution( SolutionManager *object );

//! Return the total number of solutions found. Only valid after solve( ) has returned.
inline long long SolutionManager_solution_count( SolutionManager *object )
    { return object->solutions; }

inline void SolutionManager_reset_count( SolutionManager *object )
    { object->solutions = 0; }

//! Start timing the solution process.
inline void SolutionManager_start_timer( SolutionManager *object )
    { Timer_start( &object->stopwatch ); }

//! Stop timing the solution process.
inline void SolutionManager_stop_timer( SolutionManager *object )
    { Timer_stop( &object->stopwatch ); }

#endif
