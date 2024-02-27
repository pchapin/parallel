
README
======

This folder contains different versions of a Gaussian Elimination sample program. The subfolders
are organized by programming language or parallel technology. They all solve essentially the
same problem, but in different ways.

+ C. This folder contains various versions using straight C.

+ CUDA. This folder contains the CUDA version.

+ Cpp. This folder contains a C++ version that includes a simple Matrix abstract data type
  written in a typical C++ style.
  
+ CreateSystem. This folder contains a utility program (in C++) that can be used to create large
  sample systems. The systems created have coefficients in the range (-1.0, +1.0) that are
  randomly generated. The output of this utility is in a format that is acceptable to the other
  programs.
  
+ Fortran. This folder contains a Fortran 90 implementation. Two versions are provided: a "slow"
  version that works against the memory cache, and a "fast" version that works with the cache.
  
+ MPI. This folder contains a C version using MPI for execution on a cluster.

+ Python. This folder contains a Python version using Numpy.
