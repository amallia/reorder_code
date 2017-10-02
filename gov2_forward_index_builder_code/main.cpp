#include "builder.h"
#include "merger.h"
#include <iostream>

int main(int argc, const char* argv[]){
	std::string option(argv[1]);
	/*get tuples*/
	if(option == "-gp") {
		std::cout << "generating postings\n";
		Builder builder(BuilderData::kIndexPath, BuilderData::kLexPath, BuilderData::kDocLengths, BuilderData::kDocNum);
		std::vector<Posting> postings;
		builder.GeneratePostings(1, BuilderData::kTermNum);
	}

	/*merge tuples*/
	if(option == "-mp") {
		std::cout << "merge postings\n";
		Merger merger(MergerData::kInputList);
		merger.merge();
	}

	return 0;
}