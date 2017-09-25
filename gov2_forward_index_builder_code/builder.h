#ifndef gov2_builder
#define gov2_builder

#include <iostream>
#include <string>
#include <vector>
#include <assert.h>

const std::string kIndexPath = "/home/constantinos/Datasets/GOV2/Sorted/8_1.dat_sorted";
const std::string kLexPath = "/home/constantinos/Datasets/GOV2/8_1.inf";
const std::string kDocLengths = "/home/constantinos/Datasets/GOV2/Sorted/doclen_file_sorted";
const int kDocNum = 25205179;

class Posting{
	int did;
	int tid;
	int freq;
	Posting(int d, int t, int f): did(d), tid(t), freq(f){
	}
};

class Builder{
private:
	FILE* fIndex;
	FILE* fLex;
	FILE* fDocLength;
	uint* listLengths;
	uint* offsets;
	uint* docLengths;
	uint* tmpBuffer;
public:
	/*constructor*/
	Builder(const std::string index, const std::string lex, 
		const std::string doc_length, const int doc_num);
	~Builder();
	/*get the (did, tid, freq) tuples*/
	void GetPostings(const uint term_id, std::vector<Posting>& output);
};

#endif