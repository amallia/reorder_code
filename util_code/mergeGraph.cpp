#include "Merger.h"
#include <sys/time.h>
#include <stdio.h>
#include <iostream>
using namespace std;

int main(){
	Merger* in = new Merger();
	// in->generateDeltaGraph();
	// in->loadDeltaGraph();
	// in->verifyGraph();
	// in->mergeGraph();
	// in->orderReMap();
	// in->rawQuerySelector();
	// in->queryTermIDSelector();
	// in->buildSmallLex();
	// in->docLengthRemap();
	in->buildURLOnlyGraph();
	return 0;
}