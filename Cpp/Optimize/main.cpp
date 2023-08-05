/*!
    \file   main.cpp
    \brief  Main program of the breakdown optimization problem.
    \author (C) Copyright 2011 by Peter C. Chapin <PChapin@vtc.vsc.edu>

LICENSE

This program is free software; you can redistribute it and/or modify it under the terms of the
GNU General Public License as published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if
not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
02111-1307 USA
*/

#include <cstdlib>
#include <cmath>
#include <iomanip>
#include <iostream>

#include "problem.hpp"

int main( int argc, char **argv )
{
    floating_type V;      // Breakdown voltage at current best guess.
    floating_type V_uxp;  //   ... with slight increase in u_x.
    floating_type V_uxm;  //   ... with slight decrease in u_x.
    floating_type V_uyp;  //   ... with slight increase in u_y.
    floating_type V_uym;  //   ... with slight decrease in u_y.
    floating_type V_wp;   //   ... with slight increase in w.
    floating_type V_wm;   //   ... with slight decrease in w.

    // Variables to hold gradient information
    floating_type gradient_x;
    floating_type gradient_y;
    floating_type gradient_w;
    floating_type gradient_magnitude;

    // Current best guess measured in mesh separation units.
    int u_x = 3;
    int u_y = 3;
    int w   = 400;  // For reference, half width is: (MESH_X - 1) / 2

    // Updated values.
    int u_x_new;
    int u_y_new;
    int w_new;

    // We need a proper stopping condition. How close to the maximum can we get?
    // One approach would be to work until our step size shrinks to less than one
    // mesh separation unit.
    //
    while( 1 ) {
        // Let he user know how we are doing.
        std::cout 
            <<  "(u_x = " << std::setw( 4 ) << u_x
            << ", u_y = " << std::setw( 4 ) << u_y
            << ", w = "   << std::setw( 4 ) << w << ") "  << std::flush;

        V = breakdown_voltage( u_x, u_y, w );
        std::cout << "V_breakdown = " << V << "\n";

        V_uxp = breakdown_voltage( u_x + 1, u_y,     w     );
        V_uxm = breakdown_voltage( u_x - 1, u_y,     w     );
        V_uyp = breakdown_voltage( u_x,     u_y + 1, w     );
        V_uym = breakdown_voltage( u_x,     u_y - 1, w     );
        V_wp  = breakdown_voltage( u_x,     u_y,     w + 1 );
        V_wm  = breakdown_voltage( u_x,     u_y,     w - 1 );

        // Using a quadratic interpolating polynomial would be nicer.
        // I'm too lazy to do the algebra.
        gradient_x = ( V_uxp - V_uxm ) / 2.0;  // In units of volts/mesh_separation_unit.
        gradient_y = ( V_uyp - V_uym ) / 2.0;
        gradient_w = ( V_wp  - V_wm  ) / 2.0;

        gradient_magnitude = std::sqrt( gradient_x * gradient_x + 
                                        gradient_y * gradient_y +
                                        gradient_w * gradient_w );

        // Protection. If we are exactly at the maximum we want, don't try the division.
        if( gradient_magnitude < 1.0E-09 ) break;

        // Compute a unit vector in the right direction.
        gradient_x = gradient_x / gradient_magnitude;
        gradient_y = gradient_y / gradient_magnitude;
        gradient_w = gradient_w / gradient_magnitude;

        // Always take a step of distance 10 mesh separation units (for now).
        u_x_new = u_x + int( 10.0 * gradient_x );
        u_y_new = u_y + int( 10.0 * gradient_y );
        w_new   = w   + int( 10.0 * gradient_w );

        // Check constraints.
        if( u_x_new < 2 ) u_x_new = 2;
        if( u_y_new < 2 ) u_y_new = 2;
        if( u_x_new > MESH_X - 5 ) u_x_new = MESH_X - 5;
        if( u_y_new > MESH_Y - 5 ) u_y_new = MESH_Y - 5;
        if( u_x_new + w_new > MESH_X - 3 ) w_new = MESH_X - 3 - u_x_new;
        if( u_y_new + (A / w_new) > MESH_Y - 3 ) w_new = A / (MESH_Y - 3 - u_y_new);

        // If we didn't move, we are done.
        if( u_x == u_x_new && u_y == u_y_new && w == w_new ) break;

        // Otherwise, make the step and try again.
        u_x = u_x_new;
        u_y = u_y_new;
        w   = w_new;
    }

    return EXIT_SUCCESS;
}
