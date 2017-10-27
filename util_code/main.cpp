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
	if(option == "-url") {
		Merger* m = new Merger();
		m->BuildURLOnlyGraph();
		delete m;
	}

	if(option == "-verify") {
		Merger* m = new Merger();
		m->verifyDeltaGraph(kUrlLex, kUrlIndex);
		delete m;
	}

	/*get two-terms query, with two terms that have the shortest lengths*/
	if(option == "-2tq") {
		std::cout << "generating two terms queries\n";
		LMMaker lm;
		lm.GetTwoTermQueries(kSmallLex, kQueryPool, kCommonTermFile, kTwoTermsQuery);
	}

	/*randomly get 1k queries*/
	if(option == "-r1kq") {
		std::cout << "randomly select 1k queries\n";
		LMMaker lm;
		lm.RandomGetOneKQuery(kTwoTermsQuery, kRandomQuery);
	}

	/*build small lexicon for query terms*/
	if(option == "-sl"){
		std::cout << "build small lexicon for query terms\n";
		LMMaker lm;
		lm.buildSmallLex(kQueryPool, kSmallLex);
	}

	/*generate ideal language model*/
	if(option == "-ilm") {
		LMMaker lm;
		lm.GenerateIdealLM(kRandomQuery, kIdealLMPath);
	}

	/*transform lm into termID*/
	if(option == "-lmtid") {
		LMMaker lm;
		lm.TransformLM(kTermTable, kIdealLMPath, kLMWithTermID);
	}

	if(option == "-gov2sl") {
		Gov2Utils g;
		g.Gov2SmallLex(kGov2Query,kGov2SmallLex);
	}
	return 0;
}