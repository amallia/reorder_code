# compiler flags
CC=g++
CFLAGS=-c -DNDEBUG -O3 -v -Wall -Wno-unused-variable -Wno-sign-compare -std=c++11 

all: ordering
	
ordering: main.o Lsh.o
	$(CC) main.o Lsh.o -Wl,--no-as-needed -lpthread -O3 -v -Wall -o ordering

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

Lsh.o: Lsh.cpp Lsh.h
	$(CC) $(CFLAGS) Lsh.cpp

clean:
	rm -rf *o ordering
