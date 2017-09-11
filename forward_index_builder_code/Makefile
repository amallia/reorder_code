# compiler flags
CC=g++
CFLAGS=-c -DNDEBUG -O3 -v -Wall -Wno-unused-variable -Wno-sign-compare -std=c++11 

all: graph
	
graph: main.o Term.o Document.o iterator.o Lsh.o
	$(CC) main.o Term.o Document.o iterator.o Lsh.o -Wl,--no-as-needed -lpthread -O3 -v -Wall -o graph

#-lrt

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

Term.o: Term.cpp
	$(CC) $(CFLAGS) Term.cpp

Document.o: Document.cpp Term.h globals.h
	$(CC) $(CFLAGS) Document.cpp

Lsh.o: Lsh.cpp Lsh.h globals.h iterator.h
	$(CC) $(CFLAGS) Lsh.cpp

iterator.o: iterator.cpp globals.h
	$(CC) $(CFLAGS) iterator.cpp

clean:
	rm -rf *o graph
