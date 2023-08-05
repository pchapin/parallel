#
# Makefile for the CIS-4230 Sudoku project.
#

CC=gcc
CFLAGS=-c -Wall -pthread -std=c99 -D_XOPEN_SOURCE=600 -O2 -I../../../../Spica/C
LD=gcc
LDFLAGS=-pthread
SOURCES=main.c \
        sudoku.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=Sudoku

%.o:	%.c
	$(CC) $(CFLAGS) $< -o $@

$(EXECUTABLE):	$(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -L../../../../Spica/C -lSpicaC -o $@

# File Dependencies
###################

main.o:	main.c sudoku.h

sudoku.o:	sudoku.c sudoku.h

# Additional Rules
##################
clean:
	rm -f *.o *.bc *.s *.ll *~ $(EXECUTABLE)
