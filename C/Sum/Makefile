#
# Makefile for the CIS-4230 Sum project.
#

SPICA=../../../Spica/C
CC=gcc
CFLAGS=-c -Wall -pthread -O2 -std=c99 -I$(SPICA)
LD=gcc
LDFLAGS=-pthread
LDLIBS=-L$(SPICA) -lSpicaC
SOURCES=sum.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=sum

%.o:	%.c
	$(CC) $(CFLAGS) $< -o $@

$(EXECUTABLE):	$(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o $@

# File Dependencies
###################

sum.o:	sum.c

# Additional Rules
##################
clean:
	rm -f *.o *.s *~ sum
