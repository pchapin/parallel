/****************************************************************************
FILE          : pcbuffer.h
LAST REVISION : 2008-09-15
SUBJECT       : Interface to producer/consumer module.
PROGRAMMER    : (C) Copyright 2008 by Peter C. Chapin

This header file specifies the interface to a producer/consumer buffer.

Please send comments or bug reports to

     Peter C. Chapin
     Computer Information Systems
     Vermont Technical College
     Randolph Center, VT 05061
     Peter.Chapin@vtc.vsc.edu
****************************************************************************/

#ifndef PCBUFFER_H
#define PCBUFFER_H

#include <pthread.h>

#define PCBUFFER_SIZE 8

// This is our semaphore type.
typedef struct {
  pthread_mutex_t lock;
  pthread_cond_t  non_zero;
  int             raw_count;
} semaphore;

void semaphore_init(semaphore *, int);
void semaphore_destroy(semaphore *);
void semaphore_up(semaphore *);
void semaphore_down(semaphore *);

// This is our producer/consumer buffer type.
typedef struct {
  void *buffer[PCBUFFER_SIZE];
  pthread_mutex_t lock;
  semaphore       used;      // Use our semaphores here.
  semaphore       free;      // ...
  int             next_in;   // Next available slot.
  int             next_out;  // Oldest used slot.
} pcbuffer;

void  pcbuffer_init(pcbuffer *);
void  pcbuffer_destroy(pcbuffer *);
void  pcbuffer_push(pcbuffer *, void *);
void *pcbuffer_pop(pcbuffer *);

#endif
