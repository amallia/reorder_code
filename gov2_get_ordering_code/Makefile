CXX = g++
CXXFLAG = -Wall -g -o3 -std=c++11 -lm

order: main.o tsp.o
	$(CXX) $(CXXFLAG) -o order main.o tsp.o

main.o: tsp.h
tsp.o: tsp.h

clean:
	rm *.o order