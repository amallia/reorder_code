#include "qp.h"
#include <iostream>

int main(int argc, const char* argv[]){
	std::string option(argv[1]);

	if(option == "-qp") {
		std::cout << "start query processing.. \n";
		Qp q;
		q.LoadQuery(kQuery);
		q.RunQuery();
	}
}