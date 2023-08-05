/*
 * \file shared.h
 * \brief Declarations of helper functions that are shared between the program variants.
 */

#ifndef SHARED_H
#define SHARED_H

#define SIZE  1000
#define TRUE  1
#define FALSE 0

extern double workspace[SIZE][SIZE];

void initialize_workspace( void );
void save_workspace( void );

#endif
