#include <string.h>
#include <sstream>
#include <math.h>
#include <cmath>
#include <limits>
#include <iostream>
#include <fstream>
#include <bitset>

#include "qp.h"
#include "profiling.h" 	// profiling
#include "pfor.h" 		// compression
#include "utils.h"		// helper functions

// (A) Algorithms without Block-Max structures
#include "exhaustiveOR.h"
#include "exhaustiveAND.h"
#include "Wand.h"
#include "Maxscore.h"

// (B) Algorithms with Block-Max structures
// 		(B)-1 Postings-Oriented BM algorithms
#include "PostingOriented_BMW.h"
#include "PostingOriented_BMM.h"
#include "PostingOriented_BMM_NLB.h"

using namespace std;

/* return -1 means finish, 1 means valid */
int readline(FILE* fd, char* line) {
	if(fd==NULL) {
		CERR << "input file is wrong" << Log::endl;
		exit(1);
	}

	char* tt;
	if(!feof(fd)) {
		memset(line,0,5120);
		if(fgets(line,5120,fd) == NULL) {
			//			CERR<<"end of file"<<endl;
			return -1;
		}

		if((tt=strchr(line,'\n')) == NULL) {
			CERR<<"No enter....... \n"<<Log::endl;
			CERR<<"line is "<<line<<Log::endl;
			exit(1);
		}
		*tt = ' ';
	}
	return 1;
}

/* profiling and loading lexicon and document lengths */
QueryProcessing::QueryProcessing(termsCache& cache) : qn(0), Cache(cache) {
	profilerC& p = profilerC::getInstance();
	p.add(" Total and average Query Processing time (ms): ",CONSTS::ALLQS);
	p.add(" Total and average time overhead of OTF BMG (ms): ",CONSTS::OTFBMG);
	p.add("candidateS",CONSTS::CAND); // add more values here and the CONSTS ENUM (globals.h) for counters and profiling

	for(size_t i=0; i<CONSTS::NOP; ++i)
		p.initNewCounter();

	for(size_t i=0; i<Cache.size(); ++i)
		termsInCache[std::string(Cache[i].term)]=i;
}

/* load terms -> termID into map from file */
void QueryProcessing::fillTermsMap(termsMap& lex, const std::string& path) {

	ifstream doc_lexikon_stream;
	doc_lexikon_stream.open(path.c_str());

	string lexicon_line;
	string term;
	string termid_s;
	string listLen_s;
	int listLen;
	int termid;

	while (getline(doc_lexikon_stream, lexicon_line)){

	//			cout<<"linenum: "<<linenum<<endl;
	//			if(linenum == 10)
	//				break;

				//term, first field
				string::iterator itr1 = lexicon_line.begin();
				string::iterator start1 = itr1;
				while(itr1 != lexicon_line.end() && !isspace(*itr1)){
					++itr1;
				}
				term = string(start1, itr1);
				// cout<<"term: "<<term<<" ";
				//term

				 //termID, come after the 1st space
			  	 start1 = itr1+1;
			  	 itr1++;
			  	 while (!isspace(*itr1)) {
			  	 ++itr1;
			  	 }

			  	 termid_s = string(start1,itr1);
			  	 termid = atoi(termid_s.c_str());
			  	 // cout<<"termid: "<<termid<<" ";
			  	 //termID

			  	 //listLen, come after the 2nd space
			  	 start1 = itr1+1;
			  	 itr1++;
			  	 while (!isspace(*itr1)) {
			  	 ++itr1;
			  	 }
			  	 listLen_s = string(start1, itr1);
			  	 listLen = atoi(listLen_s.c_str());
				 // cout<<"listLen: "<<listLen<<endl;

			  	 //listLen

			  	lex[string(term)] = listLen;
	}

	cout<<"termlex size: "<<lex.size()<<endl;

	doc_lexikon_stream.close();

}

/* load document length from file */
unsigned int* QueryProcessing::loadDocLen(const char* fname)	{

	int docn = CONSTS::MAXD;
	pages = new unsigned int[ docn + 128];

	ifstream inputstream;
	inputstream.open(fname);
	string curr_line_str;
	string doc_id_s;
	string doc_length_s;
	int doc_id;
	int doc_length;

	while (getline(inputstream, curr_line_str)) {
		string::iterator itr = curr_line_str.begin();
		string::iterator start = itr;

		//the first is doc id
		while(itr != curr_line_str.end() && !isspace(*itr)){
			++itr;
		}

		doc_id_s = string(start,itr);
//			  cout<<"doc_id: "<<doc_id_s<<endl;
		doc_id = atoi(doc_id_s.c_str());

		start = itr + 1;
		itr++;

		//the second is doc length
	    while (itr != curr_line_str.end()) {
			++itr;
	    }
	    doc_length_s = string(start,itr);
//			  cout<<"doc_length: "<<doc_length_s<<endl;
	    doc_length = atoi(doc_length_s.c_str());

	    pages[doc_id] = doc_length;
	}

	inputstream.close();

	for(int i =0;i<128 ; i++)
		pages[docn + i] = docn/2;

	return pages;
}

/* output of query processing: (i) result_log (docID score), (ii) kth score of result set or (iii) create ground truth for results */
class resultsDump {
	FILE *fresult_log;
	FILE *fqscore;
public:
	resultsDump(const char* result_= CONSTS::fResultLog.c_str(), const char* fresult_=CONSTS::fQueryScore.c_str()) {
		fresult_log = fopen(result_,"w");
	}
	~resultsDump() { fclose(fresult_log); }
	// dump the result for checking
	float operator()(int qid, const std::vector<std::string>& word_l, QpResult* res, int topk) {
		// where the last position that has a valid score and docid is in the topk
		int position_in_topk;
		for (position_in_topk = topk-1; position_in_topk >=0; --position_in_topk)  {
			//std::cout << position_in_topk << "\t"<< res[position_in_topk].score << "\t" << res[position_in_topk].did << std::endl;
			if (( res[position_in_topk].score > -1.0) && (res[position_in_topk].did < CONSTS::MAXD+1))
				break;
		}

		fprintf(fresult_log, "%d: ", qid);
		for (int i = 0; i<word_l.size(); i++){
			fprintf(fresult_log, "%s ", word_l[i].c_str());
		}
		fprintf(fresult_log, "\n");
		for (int i = 0 ; i <= position_in_topk; i++){
			fprintf(fresult_log,"%d,%lf\n", res[i].did, res[i].score);
		}
		// fprintf(fresult_log, "\n");
/*			// create golden standard
			FILE *Golden_threshold_handler = fopen((CONSTS::Golden_Threshold).c_str(),"a");
			for (int i = 0; i<word_l.size(); i++)
				fprintf(Golden_threshold_handler, "%s ", word_l[i].c_str());
			fprintf(Golden_threshold_handler, "%4.30f\n", res[position_in_topk].score);
			fclose(Golden_threshold_handler);
			// end of golden standard
*/
		// fprintf(fqscore,"%4.30f\n", res[position_in_topk].score);
		return res[position_in_topk].score;
	}

	void skip() {fprintf(fqscore,"0.0\n");}

};

/* Query Processing */
void QueryProcessing::operator()(const char* queryLog, const int buckets, const int limit, const int topk, const int layer)	{
	resultsDump resLogger;
	profilerC& p = profilerC::getInstance();

	this->loadDocLen(CONSTS::doclenFileName.c_str());
	// load terms -> termID into map for Standard Index
	termsMap lex;
	// fillTermsMap(lex, CONSTS::basic_table);
	this->fillTermsMap(lex, CONSTS::basic_table);
	// Standard Index
	QueryLogManager logManager(queryLog,&lex);
	// load top-10th scores for our queries - ground truth
	// logManager.loadScoresFromFile(CONSTS::Golden_Threshold.c_str());
	cout<< logManager.size() << " queries are available" << endl;

	QueryLogManager::queriesFileIterator queriesFIterator(logManager);
	queriesFIterator.changeSkipPolicy(limit,buckets);

	cout << "################################################################" << endl;
	cout << "QP configuration\ntopk:" << topk << "\tbuckets: " << buckets << "\tlimit:" << limit << "\tlayer: " << layer << endl;
	cout << "################################################################" << endl;

	/* perform query processing for each query */
	while( qn++ < 1000) {
		++queriesFIterator;
		if(queriesFIterator == logManager.end()) {
			COUT << "finished benchmark" << Log::endl;
			break;
		}

		std::vector<std::string> word_l = (*queriesFIterator);
		int qid = queriesFIterator.Getqid();
		while(word_l.size()>=10) {	// no queries more than 10 - in our query log of 1k there is just 1 query of this size
			++queriesFIterator;
			cout << "skip " << word_l << endl;
			word_l = (*queriesFIterator);
			qid = queriesFIterator.Getqid();
		}

		cout << qid << ": ";
		for(int i=0; i<word_l.size(); i++){
			cout << word_l[i] << " ";
		}
		cout << endl;

		// ########################################################################################
		// Initializing (opening) list (lps=list pointer) structures based on standard index
		lptrArray lps = openLists(word_l, lex); // Standard Index

		// ExhaustiveOR wand(pages);
		ExhaustiveAnd wand(pages);
		// Wand wand(pages);
		// ########################################################################################
		// (2) Algorithms using Posting-Oriented Block-Max Indexes
		// PostingOriented_BMW wand(pages);			 // PostingOriented_BMW - P-BMW

		// ########################################################################################
		// (D) Run algorithms (except OPT algorithms)
		QpResult res[topk]; 			// result set - comment line if PriorityArray is used to maintain the results
		p.start(CONSTS::ALLQS); 		// Start measuring qp time - NOTE: that OTF BMG is already measured if DocID-Oriented Block-Max structures are used (DOCIDBLOCKMAX is defined)

		// // various default parameters for running algorithms
		wand.anotherAnd(lps, topk, res);
		// // wand(lps, topk, 0.0f);
		// wand(lps, topk, res, 0.0f);
		// //wand(lps, topk, res, 0.0f, 0);
		// //PriorityArray<QpResult> resultsHeap = wand(lps, topk);

		p.end(CONSTS::ALLQS); 			// Stop measuring qp time - NOTE: that OTF BMG is already measured if DocID-Oriented Block-Max structures are used (DOCIDBLOCKMAX is defined)

		// // ########################################################################################
		// // obtain result set after query processing and perform sanity check with ground truth
		// // Note: depending on the algorithm the results are either returned to (a) QpResult or (b) PriorityArray<QpResult>, so comment and uncomment the next lines accordingly
		float score = resLogger(qid, word_l, res, topk);		// (a) QpResult
		// //resultsHeap.sortData();							// (b) PriorityArray<QpResult>
		// //float score = resultsHeap.getV()[topk-1].score;	//     PriorityArray<QpResult>

	} // end of Query Processing for the current Query
}

/* Print reports
Usage: 1. add to globals.h, CONSTS namespace in the counter enumerator for more counters
       2. add code for counters in the appropriate place and
	   3. use COUT3 and the name of the counter to print the results */
void QueryProcessing::printReport() const {
	profilerC& p = profilerC::getInstance();
	p.printReport();

	std::locale loc("");
	std::cout.imbue(loc);

	//COUT3 << "TOTAL EVALUATION: " << p.getCounter(CONSTS::EVAL) << Log::endl;
	//COUT3 << "Total nextGEQ: " << p.getCounter(CONSTS::NEXTGEQ) << Log::endl;
}

/* core openList function */
lptrArray QueryProcessing::openLists(const std::vector<std::string>& word_l, termsMap& lex){
	// create structure and reserve resources
	lptrArray lps;
	lps.reserve(word_l.size());
	//prepare peers
	std::vector<std::string> peers( word_l.size());

	for(size_t i=0; i<word_l.size(); i++) {
		if(termsInCache.find(word_l[i]) == termsInCache.end()){ //no term! try on-demand processing from rawindex
			// onDemandCall(word_l[i]);
			cout<< word_l[i] << endl;
			cout<<"The baby index doesn't contain this term"<<endl;
		}
		peers.push_back(Cache[ termsInCache[word_l[i]]].cpool.getName() ); //This is actually data[term].cpool.getName(), this is just adding the termpath before term. Note: termsInCache is termsMap(std::unordered_map<std::string, int>)
	}//This part of code is really confusing, but the cache part is smart

	// note: this one breaks when query has duplicate terms
	for(size_t i=0; i<word_l.size(); i++)	{
		lptr* lpsi = &(Cache[termsInCache[word_l[i]]]); //At this time, lpsi is in cache and has all the info needed to load the raw pfd data, the path is in cpool.term
		lpsi->open(peers,lex.find((word_l[i]))->second);
		lps.push_back(lpsi);
	}
	return lps;
}

/* Rule out queries not matching the given policies */
bool QueryLogManager::queriesFileIterator::isSkip() const {
	if(limit && count>=limit)
		return true;
	const size_t sz = queriesP[curr].size();
	if(sz == 0) {
		CERR << "empty query in log: " << curr << Log::endl;
		return true;
	}

	return bucket > 0 ? sz != bucket : false;
}

QueryLogManager::queriesFileIterator& QueryLogManager::queriesFileIterator::operator++() {
	++curr;
	while(curr < queriesP.size() && isSkip()) {
		++curr;
	}
	++count;
	return *this;
}

std::string joinStringsVector(const std::vector<std::string>& terms) {
	std::stringstream buffer;
	std::copy(terms.begin(), terms.end(), std::ostream_iterator<std::string>( buffer ) );
	return buffer.str();
}

float QueryLogManager::score(const std::vector<std::string>& terms) const {
	strToFloatMap::const_iterator it = mapQueryToScore.find(joinStringsVector(terms));
	return ( it != mapQueryToScore.end()) ? ((*it).second) : (-1.0);
}

void QueryLogManager::setScoreForQuery(const std::vector<std::string>& terms, float v) {
	mapQueryToScore[joinStringsVector(terms)] = v;
}

/* Load scores (Ground Truth) from file */
void QueryLogManager::loadScoresFromFile(const char* fname) {
	FILE *fq = fopen(fname,"r");
	std::vector<std::string> terms;
	while(1)  {
		terms.clear();
		char line[5120];

		if( readline(fq, line) == -1 ) //this is the end
			break;

		std::string token, text(line);
		std::istringstream iss(text);
		while ( getline(iss, token, ' ') )
			terms.push_back(token);

		//well, last one is not a term, but a score
		float score = atof(terms[terms.size()-1].c_str());
		terms.pop_back();
		setScoreForQuery(terms,score);
	}
	fclose(fq);
}

QueryLogManager::QueryLogManager(const char* fname, termsMap *l) :lex(l){
	assert(l->size());

	FILE *fq = fopen(fname,"r");
	queriesD.reserve(1000);
	std::vector<std::string> terms;
	int k = 0;
	while(1)  {
		terms.clear();
		char line[5120];

		if( readline(fq, line) == -1 ) //this is the end
			break;

		qidD.push_back(++k);//the first one is qid
		char *word;
		word = strtok(line," ");
		terms.push_back(word);
		// if( lex->find(std::string(word))!= lex->end() ) //only if term is in cache...
		// 	terms.push_back(word);
		// else
		// 	CERR << word << " is not in lex" << Log::endl;

		while((word = strtok(NULL," ")) != NULL){
			if( lex->find(std::string(word))!= lex->end() ) //only if term is in cache...
				terms.push_back(word);
			else
				CERR << word << " is not in lex" << Log::endl;
		}

		queriesD.push_back(terms);
		setScoreForQuery(terms,0.0);
	}
	//COUT << queriesD.size() << " queries loaded" << Log::endl;
	fclose(fq);

}
