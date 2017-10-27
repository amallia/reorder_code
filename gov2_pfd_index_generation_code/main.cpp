#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <string>
#include <math.h>
#include "Gov2Reader.h"
using namespace std;

int main(int argc, char * argv[] ){
   std::string option(argv[1]);
   /*go without new order*/
   if(option == "-go") {
	   Gov2Reader Reader;
	   bool order = false;
	   Reader.BuildPFDIndex(order);
   }

   /*go with new order*/
   if(option == "-goorder") {
	   Gov2Reader Reader;
	   bool order = true;
	   Reader.BuildPFDIndex(order);
   }
}
