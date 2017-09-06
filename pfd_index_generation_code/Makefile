STD=-std=c++0x -D CPP0X
DBG=$(STD) -g3 -Wall -D DEBUG
RLS=$(STD) -O3 -D NDEBUG
PROF=$(STD) -O3 -pg -D GPROFILING -D NDEBUG 

TYPE=$(PROF)
TYPE=$(DBG)
TYPE=$(RLS)

all: main.o globals.o ListIterator.o pfor.o CluewebReader.o sql/sqlite3.o
	g++  $(TYPE)  main.o globals.o pfor.o ListIterator.o  CluewebReader.o sql/sqlite3.o -o builder -lpthread
clean:
	rm builder *.o

CPPC=g++ $(TYPE) -c  

main.o: main.cpp globals.h Makefile
	$(CPPC) main.cpp

pfor.o: pfor.h pfor.cpp globals.h Makefile
	$(CPPC) pfor.cpp
 	
ListIterator.o: ListIterator.cpp ListIterator.h  BlockGens.h globals.h Makefile
	$(CPPC) ListIterator.cpp
	  	   
CluewebReader.o: CluewebReader.cpp CluewebReader.h globals.h Makefile
	$(CPPC) CluewebReader.cpp 

globals.o: globals.h globals.cpp Makefile
	$(CPPC) globals.cpp

sql/sqlite3.o: 
	gcc -c -O3 -DSQLITE_THREADSAFE=2 -DSQLITE_OMIT_LOAD_EXTENSION=1 sql/sqlite3.c -o sql/sqlite3.o

TIMING=$(STD) -O3 -D TIMING 
TYPE=$(TIMING)
