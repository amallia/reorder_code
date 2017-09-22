#include "Merger.h"
#include <sys/time.h>
#include <stdio.h>
#include <iostream>

int main(int argc, const char * argv[]){
	std::string option(argv[1]);
	// in->generateDeltaGraph();
	// in->loadDeltaGraph();
	// in->mergeGraph();
	// in->orderReMap();
	// in->rawQuerySelector();
	// in->queryTermIDSelector();
	// in->docLengthRemap();
	if (option == "-url") {
		Merger* m = new Merger();
		m->BuildURLOnlyGraph();
		delete m;
	}

	if (option == "-verify") {
		Merger* m = new Merger();
		m->verifyDeltaGraph(kUrlLex, kUrlIndex);
		delete m;
	}

	/*get two-terms query, with two terms that have the shortest lengths*/
	if (option == "-ttq") {
		std::cout << "generating two terms queries\n";
		LMMaker lm;
		lm.GetTwoTermQueries(kTwoTermsQuery);
	}

	/*build small lexicon for query terms*/
	if(option == "-sl"){
		std::cout << "build small lexicon for query terms\n";
		LMMaker lm;
		lm.buildSmallLex(kQueryPool, kSmallLex);
	}

	/*generate ideal language model*/
	if (option == "-ilm") {
		std::cout << "ideal LM maker\n";

	}

	return 0;
}