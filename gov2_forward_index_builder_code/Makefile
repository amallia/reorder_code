CXX = g++
CXXFLAGS = -Wall -g -o3 -std=c++11 -lm

build: main.o builder.o merger.o scanner.o
	$(CXX) $(CXXFLAGS) -o build main.o builder.o merger.o scanner.o

main.o: builder.h merger.h
builder.o: builder.h
merger.o: merger.h
scanner.o: scanner.h

clean:
	rm *.o build