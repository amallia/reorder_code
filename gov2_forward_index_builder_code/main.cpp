#include "builder.h"
#include <iostream>

int main(int argc, const char* argv[]){
	std::string option(argv[1]);
	/*get tuples*/
	if(option == "-gp") {
		std::cout << "generating postings\n";
		Builder builder(kIndexPath, kLexPath, kDocLengths, kDocNum);
		std::vector<Posting> postings;
		builder.GetPostings(1, postings);
	}

	/*merge tuples*/
	if(option == "-mp") {
		std::cout << "merge postings\n";
	}

	return 0;
}