#include "Merger.h"
#include <sys/time.h>
#include <stdio.h>
#include <iostream>
using namespace std;

int main(int argc, const char * argv[]){
	std::string option(argv[1]);
	Merger* in = new Merger();
	// in->generateDeltaGraph();
	// in->loadDeltaGraph();
	// in->mergeGraph();
	// in->orderReMap();
	// in->rawQuerySelector();
	// in->queryTermIDSelector();
	// in->buildSmallLex();
	// in->docLengthRemap();
	if (option == "-url") {
		in->BuildURLOnlyGraph();
	}


	if (option == "-verify") {
		in->verifyDeltaGraph(kUrlLex, kUrlIndex);
	}
	return 0;
}