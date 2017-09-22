# Makefile for Writing Make Files Example

# *****************************************************
# Variables to control Makefile operation

CXX = g++
CXXFLAGS = -Wall -g -o3 -std=c++11 -lm

# ****************************************************
# Targets needed to bring the executable up to date

utils: main.o Merger.o
	$(CXX) $(CXXFLAGS) -o utils main.o Merger.o

# The main.o target can be written more simply

main.o: Merger.h
Merger.o: Merger.h

clean:
	rm -rf *o utils