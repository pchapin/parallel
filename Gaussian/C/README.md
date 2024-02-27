
README
======

This folder contains various versions of the Gaussian Elimination sample program that are all
written in C. The variations are as follows:

+ Serial. A baseline version that uses a single thread.

+ Parallel-pthreads. A version that uses POSIX threads, but that only uses C90 features (that
  is, no variable-length arrays).
  
+ Parallel-VLA. A version that uses POSIX threads, but that uses C99-style variable-length
  arrays. This version provides more convenient access to the matrix elements and is used as a
  baseline for the other variations below.
  
