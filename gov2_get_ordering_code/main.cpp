#include "tsp.h"

int main(int argc, const char* argv[]) {
	std::string option(argv[1]);

	if(option == "-go") {
		std::cout << "Generating new did ordering for GOV2\n";
		Tsp tsp;
		tsp.Run(TspData::kForwardIndex, TspData::kLex);
	}

	if(option == "-govb") {
		std::cout << "Generating new did ordering for GOV2 with VB\n";
		TspVB tsp;
		tsp.Run(TspData::kForwardIndexVB, TspData::kLexVB);
	}
}