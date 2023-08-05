#
# Makefile for the CIS-4230 Pi project.
#

CXX=g++
CPPFLAGS=-c -Wall -pthread -D_XOPEN_SOURCE=600 -O2 -I../../../Spica/Cpp
LD=g++
LDFLAGS=-pthread
SOURCES=main.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=Pi

%.o:	%.cpp
	$(CXX) $(CPPFLAGS) $< -o $@

$(EXECUTABLE):	$(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -L../../../Spica/Cpp -lSpicaCpp -o $@

# File Dependencies
###################

main.o:		main.cpp


# Additional Rules
##################
clean:
	rm -f *.o *.bc *.s *.ll *~ $(EXECUTABLE)
