/****************************************************************************
FILE          : pth-demo1.c
LAST REVISION : 2006-08-18
SUBJECT       : Very simple GNU Portable Threads demonstration program.
PROGRAMMER    : (C) Copyright 2006 by Peter C. Chapin

Run this program and observe how it behaves. This program demonstrates the
non-preemptive nature of pth. It also shows how an entire process gets
blocked when just one thread in that process makes a blocking system call
(I/O read in this case).

Please send comments or bug reports to

     Peter C. Chapin
     Electrical and Computer Engineering Technology
     Vermont Technical College
     Randolph Center, VT 05061
     pchapin@ecet.vtc.edu
****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <pth.h>
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
  int   ch;
  pth_t threadID_1, threadID_2;

  // Initialize pth.
  if (pth_init() == FALSE) {
    fprintf(stderr, "FATAL: Unable to initialize pth!\n");
    return 1;
  }

  // Create threads.
  threadID_1 = pth_spawn(PTH_ATTR_DEFAULT, thread_1, NULL);
  threadID_2 = pth_spawn(PTH_ATTR_DEFAULT, thread_2, NULL);

  // Process the standard input.
  while ((ch = getchar()) != EOF) {
    putchar(toupper(ch));
  }

  // Wait for the other threads to terminate (which they never do).
  pth_join(threadID_1, NULL);
  pth_join(threadID_2, NULL);
  pth_exit(0);
}
