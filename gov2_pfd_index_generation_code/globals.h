/*
 * globals.h
 *
 *  Created on: Nov 19th, 2014
 *      Author: qi
 */

/*
Whatever you do, create baby lex first, use ./qp -b; Modify two places, one is the lex file dir, the other is the number of terms in the lex
*/

#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <stdlib.h>
#include <string>
#include <iostream>
#include <sstream>
#include <ostream>
#include <vector>
#include <assert.h>
#include <stdint.h>

#include <algorithm>
#include <fstream>
#include <iomanip>

using namespace std;

// typedefs
typedef unsigned int uint;
typedef unsigned long ulong;

//#define PROFILING
#ifdef GPROFILING
#define MYNOINLINE __attribute__((noinline))
#define PROFILER(a) //
#else
#define MYNOINLINE
#define PROFILER(a) profilerC::getInstance().stepCounter(a);
#endif

#ifdef CPP0X
	#include <unordered_map>
	#define hashMapStd std::unordered_map
	typedef std::unordered_map<std::string, int> termsMap;
#else
	#include <map>
	typedef std::map<std::string, int> termsMap;
	#define hashMapStd std::map
#endif




namespace Log {
	enum verbLevels { VALL, VDEBUG, VPROFILING, VOUTPUT };
#ifdef GPROFILING
#define COUT1 //
#define COUT2 //
#else
	#define COUT Log::logger()
	#define COUT1 Log::logger() << Log::verb<Log::VDEBUG>
	#define COUT2 Log::logger() << Log::verb<Log::VPROFILING>
	#define COUT3 Log::logger() << Log::verb<Log::VOUTPUT>
	#define COUT4 Log::logger(std::cout,4)
	#define COUT5 Log::logger(std::cout,5)
	#define CERR Log::logger(std::cerr,100)
	#define EFATAL Log::endl << " " << __FILE__ << " " << __LINE__ << Log::fatal
#endif
	void setGlobalVerbosityForAllLoggers(int v); //{ logger::GLOBAL_VERBOSITY = v; }

	typedef std::ostream outstrType; //just in case

	class logger {
		outstrType& out; //the stream of this logger
		int verbosity; //current verbosity
		static int GLOBAL_VERBOSITY;
	public:

		logger(outstrType& s=std::cout, int v=0) : out(s), verbosity(v) {}
		static void setGlobalV(int v) { GLOBAL_VERBOSITY = v; }
		template <typename T>
		logger& operator<<(const T& rhs) {
			if(verbosity >= GLOBAL_VERBOSITY)
			  out <<  rhs;
			return *this;
		 }

		//manipulators of the stream types
		typedef logger& (*endlType)(logger&);
		typedef logger& (*setVerbType)(logger&, int& v);
		logger& operator<<(endlType manip){ return manip(*this); }
		logger& operator<<(setVerbType manip){ return manip(*this,verbosity); }

		void setV(int v) { verbosity = v;}
		void flush() { if(verbosity >= GLOBAL_VERBOSITY) out << std::endl; }
	};

	logger& endl(logger& stream);  //stream flush
	logger& fatal(logger& stream);  //print and throw

	template<int val> //verbosity setter
	logger& verb(logger& stream, int& v) {
		v=val;
		return stream;
	}


/*

  usage
	COUT << "1 " << 2 << " " << 3.1 << log::endl << "new line" << log::endl;
	Log::setGlobalVerbosityForAllLoggers(1);
	CERR << "1 " << 2 << " " << 3.1 << log::endl << "new line" << log::endl;
	log::logger() << log::verb<5> << "1 " << 2 << " " << 3.1 << log::endl << "new line" << log::endl;
	COUT5 << "1 " << 2 << " " << 3.1 << log::endl << "new line" << log::endl;
*/

};

//input
/*system*/
// const std::string kGov2Index = "/home/constantinos/Datasets/GOV2/8_1.dat";
// const std::string kGov2Lex = "/home/constantinos/Datasets/GOV2/8_1.inf";
// const std::string kDocLenFile = "/home/constantinos/Datasets/GOV2/doclen_file";
// const std::string kSmallWordTable = "/home/qw376/reorder_data/lexicon/gov2_small_lex_1st";

/*url*/
const std::string kGov2Index = "/home/constantinos/Datasets/GOV2/Sorted/8_1.dat_sorted";
const std::string kGov2Lex = "/home/constantinos/Datasets/GOV2/8_1.inf";
const std::string kDocLenFile = "/home/constantinos/Datasets/GOV2/Sorted/doclen_file_sorted";
const std::string kSmallWordTable = "/home/qw376/reorder_data/lexicon/gov2_small_lex_2nd";

/*ordering info*/
const string kOrdering = "/home/qw376/reorder_data/ordering/ideal_ordering_1st";

//output
/*pdf index for url*/
// const string kPFDIndex = "/home/qw376/reorder_data/gov2_pfd_index_url/"; 
// const string kSqlPath = kPFDIndex + "indx.sqlite";
// const string kPFDLex = "/home/qw376/reorder_data/gov2_pfd_lex/gov2_pfd_lex_url";

/*pfd index for new ordering*/
const string kPFDIndex = "/home/qw376/reorder_data/gov2_pfd_index_order_1st/"; 
const string kSqlPath = kPFDIndex + "indx.sqlite";
const string kPFDLex = "/home/qw376/reorder_data/gov2_pfd_lex/gov2_pfd_lex_order_1st";

namespace CONSTS {
	const int MAXD = 25205179; //GOV2
	const int MAXTERMS(32818983); //GOV2
    const float AVGD(130); //GOV2
	const int MAXDBITS(25);
	const int TOPK(500);
	const int BS(64);
	/*pfd*/
	const double FRAC(0.10); //no idea what those two are...
};

typedef	std::vector<unsigned int> vecUInt;
typedef	std::vector<unsigned int>::iterator vecUIntItr;
typedef	std::vector<unsigned int>::const_iterator vecUIntCItr;

#endif /* GLOBALS_H_ */
