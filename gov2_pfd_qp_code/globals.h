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

const int kTOPK(10);
const std::string kQuery("/home/qw376/reorder_data/query/cherry_queries");
/*for running url-ordered index*/ 
const std::string kResultLogUrl("/home/qw376/reorder_data/qp_log/url_1st");
const std::string kDoclenFileNameUrl("/home/constantinos/Datasets/GOV2/Sorted/doclen_file_sorted");
const std::string kBasicTableUrl("/home/qw376/reorder_data/gov2_pfd_lex/gov2_pfd_lex_url");
const std::string kPFDIndexUrl("/home/qw376/reorder_data/gov2_pfd_index_url/");
const std::string kSqlPathUrl(kPFDIndexUrl+"indx.sqlite");
const std::string kTermPathUrl(kPFDIndexUrl+"pool");

/*for running new order index*/
const std::string kResultLogOrder("/home/qw376/reorder_data/qp_log/order_1st");
const std::string kDoclenFileNameOrder("/home/constantinos/Datasets/GOV2/Sorted/doclen_file_sorted");
const std::string kBasicTableOrder("/home/qw376/reorder_data/gov2_pfd_lex/gov2_pfd_lex_order_1st");
const std::string kPFDIndexOrder("/home/qw376/reorder_data/gov2_pfd_index_order_1st/");
const std::string kSqlPathOrder(kPFDIndexOrder+"indx.sqlite");
const std::string kTermPathOrder(kPFDIndexOrder+"pool");

namespace CONSTS {

	enum counters { CNT_EVICTIONS,
		WAND,GETBLOCKSCORE,GETBLOCKSCORE1, BLOCKFAIL,ALIGN,MISALIGNED, GETFREQ, EVAL,HEAPIFY,HEAPIFYOK,NEXTGEQ,NEXTGEQ1,ALLQS,OTFBMG,
		SKIPS, SKIPSB, DECODE, CAND, MISTRUST, ESSENTIAL, SHALLOWPOINTER, SHALLOWPOINTER1, SORT, DOCIDFILTER1, DOCIDFILTER2,
		EARLYTERMINATION0, EARLYTERMINATION01, EARLYTERMINATION1, EARLYTERMINATION2, EARLYTERMINATION3, EARLYTERMINATION4, EARLYTERMINATION5,
		NONESSENTIAL,STEP1, STEP2, STEP3, NOP };

	//######dodo########
	// const std::string ikQuery("/home/qw376/reorder_data/query/1kRandomQuery_1st");

	/*system*/
	// const std::string trecRoot("/home/qw376/reorder_data/gov2_pfd_index/");
	// const std::string doclenFileName("/home/constantinos/Datasets/GOV2/doclen_file");
	// const std::string basic_table("/home/qw376/reorder_data/gov2_pfd_lex/gov_pfd_lex_system");
	// const std::string fResultLog("/home/qw376/reorder_data/qp_log/system_1st");

	/*url*/
	// const std::string trecRoot("/home/qw376/reorder_data/gov2_pfd_index_url/");
	// const std::string doclenFileName("/home/constantinos/Datasets/GOV2/Sorted/doclen_file_sorted");
	// const std::string basic_table("/home/qw376/reorder_data/gov2_pfd_lex/gov_pfd_lex_url");
	// const std::string fResultLog("/home/qw376/reorder_data/qp_log/url_1st");
	// const std::string sqlPath(trecRoot+"indx.sqlite");

	/*new ordering*/
	const std::string trecRoot("/home/qw376/reorder_data/gov2_pfd_index_order_1st/");
	//######dodo########

	const std::string QLogPath("../QueryLog/");
	const std::string fQueryScore("../query_score");
	const std::string Golden_Threshold("../QueryLog/Golden_Threshold_trec06"); // Golden_Threshold_trec06 // Golden_Threshold_trec05
	const std::string zeroMapping("../QueryLog/10000q_terms.mapping"); //10000q_terms.mapping CHANGED !!
	const std::string termsMapping("../QueryLog/10000q_terms.mapping");
	// 10000q_terms.mapping // normal and reordered
	// 10000q_terms.mapping_layered // layered
	// 10000q_terms.mapping_layered_sorted // reordered + layered
	// version2: 10000q_terms_mapping_layered_version2 // layered (version2)
	// TREC05
	// trec05_mapping
	const std::string termsMapping_Layer("../QueryLog/10000q_terms.mapping_layered");

	// not used
	const std::string Index_term_mapping("word_file");
	const std::string entireIndexListLengths("../QueryLog/Entire_index_list_lengths");
	const std::string Unpadded_10k_postings("../QueryLog/10k_pair_term_unpadded_postings");
	const std::string Entire_Index_Maxscore_Unpadded_List_length("../QueryLog/Entire_Index_Maxscore_Unpadded_List_length"); //../QueryLog/
	const std::string Starting_Threshold("../QueryLog/10th_Starting_Threshold");
	const std::string Top1_Threshold("../QueryLog/Top-1_Threshold");
	const std::string Top10_Threshold("../QueryLog/Top-10_Threshold");
	const std::string ikQueryScores("../QueryLog/1k_scores_float");  // 1k_scores


	#define EXPECTED_BLOCKSIZE "64"
	const int EXPECTED_BLOCK_SIZE(atoi(EXPECTED_BLOCKSIZE));
	const int MAXD(25205179);
	const int MAXTERMS(32818983);
	const int MAXDBITS(25);
	const float AVGD(130);//GOV2
	const unsigned int STORAGE_CAPACITY(218); //2187 -- the lowest value to have 0 evictions on our 1K queries sample
	const int BS(64);

	const double FRAC(0.10); //no idea what those two are...
	// const int S(16);

};

typedef	std::vector<unsigned int> vecUInt;
typedef	std::vector<unsigned int>::iterator vecUIntItr;
typedef	std::vector<unsigned int>::const_iterator vecUIntCItr;


#endif /* GLOBALS_H_ */
