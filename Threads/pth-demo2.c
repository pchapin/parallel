/****************************************************************************
FILE          : pth-demo2.c
LAST REVISION : 2006-08-18
SUBJECT       : Pthreads version of pth-demo1.c
PROGRAMMER    : (C) Copyright 2006 by Peter C. Chapin

This program is just like pth-demo1 except that it uses pthreads rather
than pth (thus it really doesn't have much to do with pth). It is
instructive to compare the way this program behaves with the way pth-demo1
behaves. Note that because pthreads can be implemented non-preemptively it
is possible that this program will behave identically to pth-demo1 in some
environments.

Please send comments or bug reports to

     Peter C. Chapin
     Electrical and Computer Engineering Technology
     Vermont Technical College
     Randolph Center, VT 05061
     pchapin@ecet.vtc.edu
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

// ----------------
// Thread Functions
// ----------------

void *thread_1(void *arg)
{
  while (1) {
    printf("x\n");
    sleep(1);
  }
  return NULL;
}

void *thread_2(void *arg)
{
  while (1) {
    printf("y\n");
    sleep(1);
  }
  return NULL;
}

// ============
// Main Program
// ============

int main(void)
{
  int       ch;
  pthread_t threadID_1, threadID_2;

  // Create threads.
  pthread_create(&threadID_1, NULL, thread_1, NULL);
  pthread_create(&threadID_2, NULL, thread_2, NULL);

  // Process the standard input.
  while ((ch = getchar()) != EOF) {
    putchar(toupper(ch));
  }

  // Wait for the other threads to terminate (which they never do).
  pthread_join(threadID_1, NULL);
  pthread_join(threadID_2, NULL);
  exit(0);
}
