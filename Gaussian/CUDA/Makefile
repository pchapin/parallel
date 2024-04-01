#
# Makefile for the CIS-4230 Linear Equations project.
#

CC=gcc
NVCC=nvcc
CFLAGS=-c -std=c99 -D_XOPEN_SOURCE=600 -O2
NVCFLAGS=-c -m64 --gpu-architecture=compute_50 -D_XOPEN_SOURCE=600 -O2
LD=nvcc
LDFLAGS=-m64 --gpu-architecture=compute_50
SOURCES=solve_system.c \
        gaussian.cu    \
	Timer.c
OBJECTS=solve_system.o \
	gaussian.o     \
	Timer.o
EXECUTABLE=GaussianCUDA

%.o:	%.c
	$(CC) $(CFLAGS) $< -o $@

%.o:	%.cu
	$(NVCC) $(NVCFLAGS) $< -o $@

$(EXECUTABLE):	$(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o $@

# File Dependencies
###################

solve_system.o:	solve_system.c gaussian.h

gaussian.o:	gaussian.cu gaussian.h

Timer.o:	Timer.c Timer.h

# Additional Rules
##################
clean:
	rm -f *.o *.bc *.s *.ll *~ $(EXECUTABLE)
