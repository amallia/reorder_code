#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <string>
#include <math.h>
#include "CluewebReader.h"
using namespace std;

int main(int argc, char * argv[] ){
   CluewebReader* Reader  = new CluewebReader(CONSTS::MAXD);
   Reader->buildPFDIndex();
   delete Reader;
   return 0;
}
