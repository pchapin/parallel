#
# Makefile for the Voltage project.
#

CC=gcc
CFLAGS=-c -Wall -pthread -std=c99 -D_XOPEN_SOURCE=600 -O2
LD=gcc
LDFLAGS=-pthread
LDLIBS=
SOURCES=driver.c main-0.c main-1.c shared.c Timer.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=Voltage

%.o:	%.c
	$(CC) $(CFLAGS) $< -o $@

$(EXECUTABLE):	$(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o $@

# File Dependencies
###################

driver.o:	driver.c

main-0.o:	main-0.c shared.h Timer.h

main-1.o:	main-1.c shared.h Timer.h

shared.o:	shared.c shared.h Timer.h

Timer.o:	Timer.c Timer.h

# Additional Rules
##################
clean:
	rm -f *.o *.s *~ Voltage Voltage-results.txt
