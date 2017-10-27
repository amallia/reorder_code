CXX = g++
CXXFLAGS = -Wall -g -o3 -std=c++11 -lm

build: main.o qp.o
	$(CXX) $(CXXFLAGS) -o qp main.o qp.o

main.o: qp.h
qp.o: qp.h

clean:
	rm *.o build