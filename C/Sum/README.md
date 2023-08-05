
README
======

This folder contains the "hello world" of parallel programming: a simple program that adds all
the elements in a (large) array of double precision floating point values. Many high
performancing computing (HPC) applications that do massive scientific or enginnering
calculations manipulate large arrays of floating point values. Thus the task done by this
program is "realistic" in that sense.

The program illustrates several approaches. The first three are entirely serial and executed in
a single thread:

+ A simple serial version to use as a baseline for comparisions.

+ A fully recursive version that suffers large overheads due to the recursive process (the
  recursion goes all the way down to a base case of single element arrays).
  
+ A "hybrid" version that uses recursion at the top level, but then resorts to the simple serial
  version for large-ish base cases (current subarrays of 10,000 elements).
  
The last two approaches are parallel and executed in multiple threads:

+ A "simple" parallel version that uses exactly two threads. This is simple in the sense that
  the size of various data structures can be statically allocated, making the code a little
  clearer. However, this version does not take advantage of any additional threads that might be
  available.
  
+ A full parallel version that queries the operating system for information about the available
  processors and scales itself to use all of them. This entials the dynamic allocation of
  various data structures using a size equal to the number of processors (learned only during
  execution).
  
This program requires the use of a third-party timer library. Get the Spica library from here:

    https://github.com/pchapin/spica
    
Clone it to your system. Then go into Spica/C and execute `make` to build the C version of the
library. Modify the Makefile here to include the path to the Spica/C folder on your system
before trying to build this sample.

Enjoy!

Peter Chapin  
pchapin@vtc.edu  
