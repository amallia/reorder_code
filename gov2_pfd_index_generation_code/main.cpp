#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <string>
#include <math.h>
#include "Gov2Reader.h"
using namespace std;

int main(int argc, char * argv[] ){
   Gov2Reader* Reader  = new Gov2Reader(CONSTS::MAXD);
   Reader->buildPFDIndex();
   delete Reader;
   return 0;
}
