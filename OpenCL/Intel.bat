@echo off

REM The following command compiles the OpenCL sample program using Intel's oneAPI compilers.

icx "-IC:\Program Files (x86)\Intel\oneAPI\compiler\latest\include\sycl" ^
    VectorAdd.c ^
    /link "/LIBPATH:C:\Program Files (x86)\Intel\oneAPI\compiler\latest\lib" OpenCL.lib
