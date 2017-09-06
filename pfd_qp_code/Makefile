STD=-std=c++0x -D CPP0X
DBG=$(STD) -g3 -Wall -D DEBUG
RLS=$(STD) -O3 -D NDEBUG

PROF=$(STD) -O3 -pg -D GPROFILING -D NDEBUG 
#-fno-inline
#-ffast-math -fomit-frame-pointer -finline-functions -finline-limit=1200 -fstrength-reduce -fexpensive-optimizations -funroll-loops  
 
TYPE=$(PROF)
TYPE=$(DBG)
TYPE=$(RLS)

# all: main.o ListIterator.o profiling.o main.cpp qp.o pfor.o DocidOriented_BMW_BMQ.o PostingOriented_BMM_NLB.o DocidOriented_BMM_BMQ.o PostingOriented_BMW.o DocidOriented_BMM_Layering.o TrecReader.o DocidOriented_BMM.o Wand.o globals.o DocidOriented_BMW.o exhaustiveOR.o PostingOriented_BMM.o Maxscore.o sql/sqlite3.o
# 	g++  $(TYPE)  main.o globals.o pfor.o qp.o ListIterator.o profiling.o PostingOriented_BMM_NLB.o DocidOriented_BMW_BMQ.o DocidOriented_BMM_BMQ.o DocidOriented_BMM_Layering.o PostingOriented_BMW.o DocidOriented_BMM.o Wand.o TrecReader.o DocidOriented_BMW.o exhaustiveOR.o Maxscore.o PostingOriented_BMM.o sql/sqlite3.o -o qp -lpthread
all: main.o ListIterator.o profiling.o main.cpp qp.o pfor.o PostingOriented_BMW.o globals.o Wand.o exhaustiveOR.o exhaustiveAND.o sql/sqlite3.o
	g++  $(TYPE)  main.o globals.o pfor.o qp.o ListIterator.o profiling.o PostingOriented_BMW.o Wand.o exhaustiveOR.o exhaustiveAND.o sql/sqlite3.o -o qp -lpthread
# -ldl 
clean:
	rm qp *.o

CPPC=g++ $(TYPE) -c  

main.o: main.cpp qp.h globals.h Makefile
	$(CPPC) main.cpp

PostingOriented_BMW.o: PostingOriented_BMW.h PostingOriented_BMW.cpp ListIterator.h globals.h Makefile
	$(CPPC) PostingOriented_BMW.cpp
	
Wand.o: Wand.h Wand.cpp ListIterator.h globals.h Makefile
	$(CPPC) Wand.cpp

exhaustiveAND.o: exhaustiveAND.h exhaustiveAND.cpp ListIterator.h globals.h utils.h Makefile
	$(CPPC) exhaustiveAND.cpp
	
exhaustiveOR.o: exhaustiveOR.h exhaustiveOR.cpp ListIterator.h globals.h utils.h Makefile
	$(CPPC) exhaustiveOR.cpp	
		
qp.o: qp.h qp.cpp PostingOriented_BMW.h exhaustiveOR.h PostingOriented_BMM_NLB.h Maxscore.h PostingOriented_BMM.h Wand.h pfor.h globals.h ListIterator.h BlockGens.h utils.h Makefile 
	$(CPPC) qp.cpp

pfor.o: pfor.h pfor.cpp globals.h Makefile
	$(CPPC) pfor.cpp
 	
profiling.o: profiling.cpp 	profiling.h globals.h Makefile
	$(CPPC) profiling.cpp
	
ListIterator.o: ListIterator.cpp ListIterator.h  BlockGens.h globals.h Makefile
	$(CPPC) ListIterator.cpp
	
globals.o: globals.h globals.cpp Makefile
	$(CPPC) globals.cpp
		
sql/sqlite3.o: 
	gcc -c -O3 -DSQLITE_THREADSAFE=2 -DSQLITE_OMIT_LOAD_EXTENSION=1 sql/sqlite3.c -o sql/sqlite3.o

TIMING=$(STD) -O3 -D TIMING 
TYPE=$(TIMING)
