#
# Makefile for the CIS-4230 QuickSort project.
#

CC=gcc
CFLAGS=-c -Wall -pthread -std=c99 -D_XOPEN_SOURCE=600 -O2 -I../../../Projects/Spica/C
LD=gcc
LDFLAGS=-pthread
LDLIBS=-L../../../Projects/Spica/C -lSpicaC
SOURCES=main.c QuickSort.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=QuickSort

%.bc:	%.c
	$(CC) $(CFLAGS) $< -o $@

$(EXECUTABLE):	$(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o $@

# File Dependencies
###################

main.bc:	main.c QuickSort.h

QuickSort.bc:	QuickSort.c QuickSort.h

# Additional Rules
##################
clean:
	rm -f *.o *.bc *.s *.ll *~ $(EXECUTABLE)
