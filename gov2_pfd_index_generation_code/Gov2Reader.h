#ifdef __APPLE__
#  define off64_t off_t
#  define fopen64 fopen
#endif

#ifndef _GOV2_READER_
#define _GOV2_READER_

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

class Gov2Reader{
public:
// private:
	int small_num_terms;
	int num_terms;
	int docn;
	uint* doclen;
	uint* lex_buffer;
	uint* lex_prefix;
	uint* hold_buffer;
	vector<string> terms;
	vector<uint> tids;
	map<int, int> list_len_map;

	SqlProxy sql;
	FILE* findex;
  	unordered_map<int, int> order_map;

	// load the lex into mem, get the urls and doclength
	~Gov2Reader();
	void BuildPFDIndex(bool new_order);
	void LoadIndex();
	void LoadDocLength(int docn);
	void getDidTfs(RawIndexList& tList);
	// Usage: Given the term and its term_id, return a RawIndexList structure (vector of scores, dids, freqs) - padded
	RawIndexList loadRawList(const std::string& term,
	size_t wid, bool new_order);
	int decompressionVarBytes(unsigned char* input, unsigned int* output, int size);
	void printBinary(unsigned int num);
	void LoadOrdering(const std::string order_file);
	void applyNewOrderingToDidTfs(RawIndexList& tList);
	void WriteOutNewLex();
};


#endif
