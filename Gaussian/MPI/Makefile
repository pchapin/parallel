#
# Makefile for the CIS-4230 Linear Equations project (MPI version).
#

CC=mpicc
CFLAGS=-c -fopenmp -std=gnu99 -O -I../../../../Spica/C
LD=mpicc
LDFLAGS=-fopenmp
SOURCES=solve_system.c linear_equations.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=LinEqMPI

%.o:	%.c
	$(CC) $(CFLAGS) $< -o $@

$(EXECUTABLE):	$(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -L../../../../Spica/C -lSpicaC -lm -o $@

# File Dependencies
###################

solve_system.o:	            solve_system.c linear_equations.h

linear_equations.o:         linear_equations.c linear_equations.h

# Additional Rules
##################
clean:
	rm -f *.o *.bc *.s *.ll *~ $(EXECUTABLE)
