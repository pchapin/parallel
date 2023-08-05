/*!
    \file   problem.hpp
    \brief  Header file containing various contants that define the problem.
    \author (C) Copyright 2011 by Peter C. Chapin <PChapin@vtc.vsc.edu>

    In the future it would be nicer to take this information from a configuration file.
*/

#ifndef PROBLEM_HPP
#define PROBLEM_HPP

// Change here to adjust the precision used in the computation.
typedef double floating_type;

// The dimensions of the rectangular enclosing tube (in meters).
const floating_type W = 2.0E-02;   // 2 cm.
const floating_type H = 1.0E-02;   // 1 cm.

// The number of mesh points in each direction. NOTE: mesh must be uniform! If you change the
// aspect ratio of the tube, you must change here. The width of the tube is MESH_X - 1 mesh
// separation units and the height is MESH_Y - 1 mesh separation units.
//
const int MESH_X = 1001;  // Includes boundaries. MESH_X - 2 interior points.
const int MESH_Y =  501;  // Includes boundaries. MESH_Y - 2 interior points.

// Cross sectional area of internal structure in square mesh separation units.
const int A = ( MESH_X/2 ) * ( MESH_Y/2 );

// E field strength for breakdown (V/m).
const floating_type E_BREAKDOWN = 3.0E+06;

// Computes the breakdown voltage for a given configuration of the internal structure. The
// dimensions used below are in units of mesh separation units. The mesh density is exposed to
// the caller so that no attempt is made to move a fraction of a mesh separation.
//
extern floating_type breakdown_voltage( int u_x, int u_y, int w );

#endif
