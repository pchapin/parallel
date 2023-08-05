#
# Makefile for the OpenMP sample program.
#

CC=gcc
CFLAGS=-c -std=c99 -D_XOPEN_SOURCE=600 -fopenmp -O2
LD=gcc
LDFLAGS=-fopenmp
SOURCES=main.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=OpenMP

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
