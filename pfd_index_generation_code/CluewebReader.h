#ifdef __APPLE__
#  define off64_t off_t
#  define fopen64 fopen
#endif

#ifndef _CLUEWEB_READER_
#define _CLUEWEB_READER_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <map>
#include "globals.h"
#include "ListIterator.h"
#include "SqlProxy.h"

using namespace std;

class qpRunnerPosting{
public:
  uint did;
  uint freq;
  qpRunnerPosting(uint d, uint f){
    this->did = d;
    this->freq = f;
  }
};

class CluewebReader{
public:
	CluewebReader() {}
	// load the lex into mem, get the urls and doclength
	CluewebReader(int docn);
	void getDidTfs(RawIndexList& tList);
	// Usage: Given the term and its term_id, return a RawIndexList structure (vector of scores, dids, freqs) - padded
	RawIndexList loadRawList(const std::string& term, size_t wid);
	void buildPFDIndex();
	int decompressionVarBytes(unsigned char* input, unsigned int* output, int size);
	void printBinary(unsigned int num);
	~CluewebReader(void);
// private:
	FILE *findex;
	FILE *finf;
	FILE *flex;
	FILE *fdoclength;

	int docn;
	unsigned int* doclen;
	SqlProxy sql;

//	int currentwid;
	unsigned char * compressed_list;
	unsigned int *uncompressed_list;

	//for baby_lex
	vector<string> term_;
	map <string, int> term_map;
	map <int, string> termIDMap;
	unsigned int* termid_;
	unsigned long* listLen_;
	unsigned long* offset_;
	unsigned long* listLenBytes_;


	size_t* inf_prefix_sum;
#ifdef CPP0X
	std::unordered_map<std::string, int*> lex;
#else
	std::map<std::string, int*> lex;
#endif

	//CIKM 2017
	const string cluewebIndex = "/home/qw376/Info_Clueweb/InvertedIndex";//clueweb whole index
	const string smallLex = "/home/qw376/CIKM2017/lexFiles/1kQueriesLex406Uniq"; //lexicon 

 	// const string ordering = "/home/qw376/SIGIR2017/didReassignment/tspOrder318";
	// const string ordering = "/home/qw376/SIGIR2017/didReassignment/randomOrder";
	// const string ordering = "/home/qw376/SIGIR2017/didReassignment/urlOrder";
	const string ordering = "/home/qw376/SIGIR2017/didReassignment/tspOrder323";

	// const string docLenFileDir = "/home/qw376/Info_Clueweb/documentLengths"; //doc lengths
	// const string docLenFileDir = "/home/qw376/CIKM2017/docLengthFiles/randomDocLength"; 
	// const string docLenFileDir = "/home/qw376/CIKM2017/docLengthFiles/URLDocLength"; 
	const string docLenFileDir = "/home/qw376/CIKM2017/docLengthFiles/TSPLSHURLDocLength"; 

  	// const string smallIndexDir= "/home/qw376/CIKM2017/smallIndexRandom/"; 
  	const string smallIndexDir = "/home/qw376/CIKM2017/smallIndexTSPLSH/"; 
  	// const string smallIndexDir = "/home/qw376/CIKM2017/smallIndexURL/"; 
  	// const string smallIndexDir = "/home/qw376/CIKM2017/smallIndexTSPLSHURL/"; 

  	// const string pfdLex = "/home/qw376/CIKM2017/PFDlexFiles/random1kQueriesLex406Uniq";
  	// const string pfdLex = "/home/qw376/CIKM2017/PFDlexFiles/URL1kQueriesLex406Uniq";
  	const string pfdLex = "/home/qw376/CIKM2017/PFDlexFiles/TSPLSHURL1kQueriesLex406Uniq";

  	uint numTerms;
  	uint numberOfTerms = 3000;//make sure this is larger than the lex size
  	unordered_map<unsigned int, unsigned int> newOrderMap;

  	void loadNewOrdering();
	void loadDocLength();
	void applyNewOrderingToDidTfs(RawIndexList& tList);
	void updateLex(RawIndexList& tList);
	void writeOutNewLex();
};


#endif
