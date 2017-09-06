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

namespace CONSTS {

	enum counters { CNT_EVICTIONS,
		WAND,GETBLOCKSCORE,GETBLOCKSCORE1, BLOCKFAIL,ALIGN,MISALIGNED, GETFREQ, EVAL,HEAPIFY,HEAPIFYOK,NEXTGEQ,NEXTGEQ1,ALLQS,OTFBMG,
		SKIPS, SKIPSB, DECODE, CAND, MISTRUST, ESSENTIAL, SHALLOWPOINTER, SHALLOWPOINTER1, SORT, DOCIDFILTER1, DOCIDFILTER2,
		EARLYTERMINATION0, EARLYTERMINATION01, EARLYTERMINATION1, EARLYTERMINATION2, EARLYTERMINATION3, EARLYTERMINATION4, EARLYTERMINATION5,
		NONESSENTIAL,STEP1, STEP2, STEP3, NOP };

	const std::string trecRoot("/home/qw376/CIKM2017/smallIndexTSPLSHURL/"); //main index 
	// const std::string trecRoot("/home/qw376/CIKM2017/smallIndexURL/"); //main index 
	const std::string sqlPath(trecRoot+"indx.sqlite");


	#define EXPECTED_BLOCKSIZE "64"
	const int EXPECTED_BLOCK_SIZE(atoi(EXPECTED_BLOCKSIZE));


	const std::string TERM_PATH(trecRoot+"pool");
	const std::string FLAG_PATH(trecRoot+"flag");
	const std::string MAX_PATH(trecRoot+"max");
	const std::string SCORE_PATH(trecRoot+"score");
	const std::string SIZE_PATH(trecRoot+"size");

	const std::string GOOD_TERM_SUFFIX("_good");
	const std::string BAD_TERM_SUFFIX("_bad");

	const std::string INFO_INDEX("8_1.inf");
	const std::string INDEX("8_1.dat");
	const std::string SORTED("_sorted");
	const std::string DOCUMENT_LENGTH("doclen_file");
	const std::string MERGED_BOOL_PATH("merged_bool/");
	const std::string WORD_FILE("/home/constantinos/Trec06_RawIndx/word_file");

//	const int MAXD(25205179); //trec
	const int MAXD(50220423); //clue_web
//	const int MAXTERMS(32818983); //trec
	const int MAXTERMS(86532822); //clue_web
    const float AVGD(860.917); //clue_web
	const int MAXDBITS(25);
	const int TOPK(500);
	const unsigned int STORAGE_CAPACITY(218); //2187 -- the lowest value to have 0 evictions on our 1K queries sample
	const int BS(64);

	const unsigned int bitvector_size = 1<<15;

	// Quantization
	const int Quantization(255);

	// Layering
	const int LAYER_SPLIT_POSTINGS_THRESHOLD(4096); // 65536 : Version1 (BMW): 50000 // Version2: 4096
	const float LAYER_SCORE_PARAMETER(0.25); // Version1 (BMW): 0.02  // Version2: 0.25  //0.5f // 0.25f

	const double FRAC(0.10); //no idea what those two are...
	const int S(16);

};

typedef	std::vector<unsigned int> vecUInt;
typedef	std::vector<unsigned int>::iterator vecUIntItr;
typedef	std::vector<unsigned int>::const_iterator vecUIntCItr;

#endif /* GLOBALS_H_ */
