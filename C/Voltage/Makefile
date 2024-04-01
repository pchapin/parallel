#
# Makefile for the Voltage project.
#

CC=gcc
CFLAGS=-std=c99 -Wall -D_XOPEN_SOURCE=600 -O3
LD=mpicc
LDFLAGS=-fopenmp
LDLIBS=
SOURCES=driver.c main-0.c main-1.c main-2.c main-3.c shared.c Timer.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=Voltage

%.o:	%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(EXECUTABLE):	$(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o $@

# File Dependencies
###################

driver.o:	driver.c

main-0.o:	main-0.c shared.h Timer.h environ.h

main-1.o:	main-1.c shared.h Timer.h environ.h

main-2.o:	main-2.c shared.h Timer.h environ.h
	mpicc $(CFLAGS) -fopenmp -c $< -o $@

main-3.o:	main-2.c shared.h Timer.h environ.h

shared.o:	shared.c shared.h Timer.h environ.h

Timer.o:	Timer.c Timer.h

# Additional Rules
##################
clean:
	rm -f *.o *.s *~ Voltage Voltage-results.txt
