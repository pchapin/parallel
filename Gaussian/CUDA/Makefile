#
# Makefile for the CIS-4230 Linear Equations project.
#

CC=gcc
NVCC=nvcc
CFLAGS=-c -std=c99 -D_XOPEN_SOURCE=600 -O2 -I../../../Spica/C
NVCFLAGS=-c -m64 -arch=sm_13 -D_XOPEN_SOURCE=600 -O2 -I../../../Spica/C
LD=nvcc
LDFLAGS=-m64 -arch=sm_13
SOURCES=solve_system.c \
        linear_equations.cu
OBJECTS=solve_system.o \
	linear_equations.o
EXECUTABLE=LinEq

%.o:	%.c
	$(CC) $(CFLAGS) $< -o $@

%.o:	%.cu
	$(NVCC) $(NVCFLAGS) $< -o $@

$(EXECUTABLE):	$(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -L../../../Spica/C -lSpicaC -o $@

# File Dependencies
###################

solve_system.o:	solve_system.c linear_equations.h

linear_equations.o:	linear_equations.cu linear_equations.h

# Additional Rules
##################
clean:
	rm -f *.o *.bc *.s *.ll *~ $(EXECUTABLE)
