CXX = g++
CXXFLAGS = -Wall -g -o3 -std=c++11 -lm

build: main.o builder.o
	$(CXX) $(CXXFLAGS) -o build main.o builder.o

main.o: builder.h
builder.o: builder.h

clean:
	rm *.o build