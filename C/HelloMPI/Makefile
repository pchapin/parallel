#
# Makefile for the HelloMPI program. Use this as a skeleton for other MPI applications.
#

CC=mpicc
CFLAGS=-c -std=c99 -D_XOPEN_SOURCE=600 -O3
LD=mpicc
LDFLAGS=
SOURCES=main.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=main

%.o:	%.c
	$(CC) $(CFLAGS) $< -o $@

$(EXECUTABLE):	$(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -lm -o $@

# File Dependencies
###################

main.o:         main.c

# Additional Rules
##################
clean:
	rm -f *.o *.bc *.s *.ll *~ $(EXECUTABLE)
