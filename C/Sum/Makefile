#
# Makefile for the CIS-4230 Sum project.
#

CC=gcc
CFLAGS=-c -std=c99 -Wall -pthread -O3
LD=gcc
LDFLAGS=-pthread
SOURCES=sum.c Timer.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=sum

%.o:	%.c
	$(CC) $(CFLAGS) $< -o $@

$(EXECUTABLE):	$(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o $@

# File Dependencies
###################

sum.o:		sum.c Timer.h environ.h

Timer.o:	Timer.c environ.h

# Additional Rules
##################
clean:
	rm -f *.o *.s *~ sum
