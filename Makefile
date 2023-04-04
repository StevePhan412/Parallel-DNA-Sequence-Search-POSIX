CXX = g++
CXXFLAGS = -Wall
LDLIBS = -lpthread

PROGRAM=program1

all: $(PROGRAM)

$(PROGRAM): Assignment1.cpp
	$(CXX) $(CXXFLAGS) $< $(LDLIBS) -o $@
