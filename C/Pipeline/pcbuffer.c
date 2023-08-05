/****************************************************************************
FILE          : pcbuffer.c
LAST REVISION : 2008-09-15
SUBJECT       : Implementation of producer/consumer module.
PROGRAMMER    : (C) Copyright 2008 by Peter C. Chapin

Please send comments or bug reports to

     Peter C. Chapin
     Computer Information Systems
     Vermont Technical College
     Randolph Center, VT 05061
     Peter.Chapin@vtc.vsc.edu
****************************************************************************/

#include "pcbuffer.h"

// Implementation of my semaphore type.

void semaphore_init(semaphore *p, int initial)
{
  if (initial < 0) initial = 0;

  p->raw_count = initial;
  pthread_mutex_init(&p->lock, 0);
  pthread_cond_init(&p->non_zero, 0);
}


void semaphore_destroy(semaphore *p)
{
  pthread_mutex_destroy(&p->lock);
  pthread_cond_destroy(&p->non_zero);
}


void semaphore_up(semaphore *p)
{
  pthread_mutex_lock(&p->lock);
  p->raw_count++;
  pthread_mutex_unlock(&p->lock);
  pthread_cond_signal(&p->non_zero);
}


void semaphore_down(semaphore *p)
{
  pthread_mutex_lock(&p->lock);
  while (p->raw_count == 0)
    pthread_cond_wait(&p->non_zero, &p->lock);

  p->raw_count--;
  pthread_mutex_unlock(&p->lock);
}

// Implementation of my producer/consumer buffer type.

void pcbuffer_init(pcbuffer *p)
{
  pthread_mutex_init(&p->lock, NULL);
  semaphore_init(&p->used, 0);
  semaphore_init(&p->free, PCBUFFER_SIZE);
  p->next_in = p->next_out = 0;
}


void pcbuffer_destroy(pcbuffer *p)
{
  pthread_mutex_destroy(&p->lock);
  semaphore_destroy(&p->used);
  semaphore_destroy(&p->free);
}


void pcbuffer_push(pcbuffer *p, void *incoming)
{
  semaphore_down(&p->free);
  pthread_mutex_lock(&p->lock);
  p->buffer[p->next_in] = incoming;
  p->next_in++;
  if (p->next_in >= PCBUFFER_SIZE) p->next_in = 0;
  pthread_mutex_unlock(&p->lock);
  semaphore_up(&p->used);
}


void *pcbuffer_pop(pcbuffer *p)
{
  void *return_value;

  semaphore_down(&p->used);
  pthread_mutex_lock(&p->lock);
  return_value = p->buffer[p->next_out];
  p->next_out++;
  if (p->next_out >= PCBUFFER_SIZE) p->next_out = 0;
  pthread_mutex_unlock(&p->lock);
  semaphore_up(&p->free);

  return return_value;
}
