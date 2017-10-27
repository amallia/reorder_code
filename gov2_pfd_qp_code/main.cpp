#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include "qp.h"
#include "optionparser.h"

 enum  optionIndex { UNKNOWN, HELP, BUILD, ONDEMAND, OUTPATH, QP, QPATH, OFFSET, LIMIT, VERB, TOPK, LAYER, INDEXROOT };
 const option::Descriptor usage[] =
 {
  {UNKNOWN, 	0,"" , ""    ,		option::Arg::None, "USAGE: example [options]\n\nOptions:" },
  {HELP,    	0,"h", "help",		option::Arg::None, "  --help  \tPrint usage and exit." },
  {BUILD,   	0,"b", "buildindex",option::Arg::Optional, "  --buildindex[=<raw index path>], -b [<raw index path>]  \tBuild index with optional path to raw index" },
  {ONDEMAND,   	0,"d", "demandBld", option::Arg::Optional, "  --demandBld[=<raw index path>], -d [<raw index path>]  \tBuild term lists on demand with optional path to raw index" },
  {OUTPATH,   	0,"o", "outputpath",option::Arg::Optional, "  --outputpath[=<index path>], -o [<index path>]  \tWhere to store the index" },
  {QPATH,   	0,"p", "qpath",     option::Arg::Optional, "  --qpath[=<query log path>], -p [<query log path>]  \tWhere to get the queries from" },
  {QP,   		0,"q", "qp",		option::Arg::Optional, "  -q [<buckets>], \t--qp[=<buckets>] \tRun query proc. with optional buckets" },
  {LIMIT,  		0,"l", "limit",		option::Arg::Optional, "  -l [<limit>], \t--limit[=<limit>] \tRun at most limit queries" },
  {OFFSET, 		0,"s", "start",		option::Arg::Optional, "  -s [start], \t--start[=<start>] \tThe first index to start at -> for build, but when run selects the exp. block size" },
  {VERB, 		0,"v", "verbosity",	option::Arg::Optional, "  -v [int], \t--verbosity[=int] \tLogging level -- lower level - more messages" },
  {TOPK, 		0,"k", "topk",	    option::Arg::Optional, "  -k [int], \t-- number of top-k documents to retrieve" },
  {LAYER, 		0,"r", "layer",		option::Arg::Optional, "  -r [int], \t-- Layer off: 0, Layer on: 1" },
  {INDEXROOT, 	0,"i", "indexroot",	option::Arg::Optional, "  -i [str], \t--indexroot[=str] \tThe path to trecroot" },
  {0,0,0,0,0,0}
 };


int main(int argc, char * argv[] ){
	   std::string option(argv[1]);
	   termsCache Cache;
	   if(option == "-run_url") {
	   	 Cache.fill_cache(kSqlPathUrl,kTermPathUrl); // this does all the term reading
	   	 QueryProcessing qp(Cache);
	     qp(kQuery.c_str(), kTOPK, 
	     kResultLogUrl, kDoclenFileNameUrl,
	     kBasicTableUrl); //process the queries
	     qp.printReport();
	   }
	   if(option == "-run_order") {
	   	 Cache.fill_cache(kSqlPathOrder,kTermPathOrder); // this does all the term reading
	   	 QueryProcessing qp(Cache);
	     qp(kQuery.c_str(), kTOPK, 
	     kResultLogOrder, kDoclenFileNameOrder,
	     kBasicTableOrder); //process the queries
	     qp.printReport();
	   }

	   return 0;
}

