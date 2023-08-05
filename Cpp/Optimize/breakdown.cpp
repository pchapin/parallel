/*!
    \file   breakdown.cpp
    \brief  A function that computes the breakdown voltage of a particular configuration.
    \author (C) Copyright 2011 by Peter C. Chapin <PChapin@vtc.vsc.edu>
*/

#include <cmath>
#include "problem.hpp"

// For optimal cache utilization, scan in Y direction "fastest" (in inner loops).
floating_type voltage_mesh[MESH_X][MESH_Y];
floating_type efield_mesh [MESH_X][MESH_Y];

static bool on_structure( int u_x, int u_y, int w, int i, int j )
{
    if( ( i == u_x     && j >= u_y && j <= (u_y + A/w) ) ||    // Left side.
        ( i == u_x + w && j >= u_y && j <= (u_y + A/w) ) ||    // Right side.
        ( j == u_y     && i >= u_x && i <= (u_x + w  ) ) ) {   // Bottom.
            return true;
    }
    return false;
}


// The following function zeros the mesh and sets up the internal structure.
static void initialize_voltage_mesh( int u_x, int u_y, int w )
{
    for( int i = 0; i < MESH_X; ++i ) {
        for( int j = 0; j < MESH_Y; ++j ) {
            voltage_mesh[i][j] = 0.0;
            efield_mesh [i][j] = 0.0;

            if( on_structure( u_x, u_y, w, i, j ) ) {
                voltage_mesh[i][j] = 1.0;
            }
        }
    }
}


// The following function passes over the mesh and updates the voltage at each point.
static void execute_iteration( int u_x, int u_y, int w )
{
    // Scan over all interior points.
    for( int i = 1; i < MESH_X - 1; ++i ) {
        for( int j = 1; j < MESH_Y - 1; ++j ) {
            if( on_structure( u_x, u_y, w, i, j ) ) continue;
            voltage_mesh[i][j] = ( voltage_mesh[i + 1][j] +
                                   voltage_mesh[i - 1][j] +
                                   voltage_mesh[i][j + 1] +
                                   voltage_mesh[i][j - 1] ) / 4.0;
        }
    }
}


// The following function computes the magnitude of the electric field at each point.
static void compute_efield( int u_x, int u_y, int w )
{
    const floating_type mesh_separation = W / floating_type( MESH_X - 1 );
    floating_type gradient_x;
    floating_type gradient_y;
    floating_type gradient_magnitude;

    // Scan over all interior points.
    for( int i = 1; i < MESH_X - 1; ++i ) {
        for( int j = 1; j < MESH_Y - 1; ++j ) {
            if( on_structure( u_x, u_y, w, i, j ) ) continue;
            gradient_x = 
                ( voltage_mesh[i + 1][j] - voltage_mesh[i - 1][j] ) / ( 2.0 * mesh_separation );
            gradient_y =
                ( voltage_mesh[i][j + 1] - voltage_mesh[i][j - 1] ) / ( 2.0 * mesh_separation );
            gradient_magnitude = std::sqrt( gradient_x * gradient_x + gradient_y * gradient_y );
            efield_mesh[i][j] = gradient_magnitude;
        }
    }
}


// The following function returns the maximum efield in the mesh.
floating_type maximum_efield( int u_x, int u_y, int w )
{
    floating_type max = 0.0;

    // Scan over all interior points.
    for( int i = 1; i < MESH_X - 1; ++i ) {
        for( int j = 1; j < MESH_Y - 1; ++j ) {
            if( !on_structure( u_x, u_y, w, i, j ) && efield_mesh[i][j] > max )
                max = efield_mesh[i][j];
        }
    }
    return max;
}


floating_type breakdown_voltage( int u_x, int u_y, int w )
{
    // Put a bound on the number of iterations as a safety. In a real program this value
    // should be set to a high number and used as an escape valve to prevent the program
    // from running away if there is an error of some kind. Normal execution should break
    // out of the loop below with a break statement on a suitable stopping condition.
    //
    const int MAX = 2 * ( (MESH_X > MESH_Y) ? MESH_X : MESH_Y );

    initialize_voltage_mesh( u_x, u_y, w );

    // Loop until the iterations converge. (A better stopping condition could be imagined)
    for( int i = 0; i < 2*MAX; ++i ) {
        execute_iteration( u_x, u_y, w );
    }
    
    compute_efield( u_x, u_y, w );
    return E_BREAKDOWN / maximum_efield( u_x, u_y, w );
}
