#ifndef GOV2_BUILDER
#define GOV2_BUILDER

#include <iostream>
#include <string>
#include <vector>
#include <assert.h>

namespace BuilderData{
	const std::string kIndexPath = "/home/constantinos/Datasets/GOV2/Sorted/8_1.dat_sorted";
	const std::string kLexPath = "/home/constantinos/Datasets/GOV2/8_1.inf";
	const std::string kDocLengths = "/home/constantinos/Datasets/GOV2/Sorted/doclen_file_sorted";
	const std::string kBufferDir = "/home/qw376/reorder_data/tmp_buffer_data_gov2/b";
	const int kBufferSize = 1024*1024*2000; //the actual file size will be *8, 16G
	const int kDocNum = 25205179; //max number of documents
	const int kTermNum = 32818983; //max number of terms
}

class Posting{
public:	
	int did;
	int tid;
	// int freq;
	// Posting(int d, int t, int f): did(d), tid(t), freq(f){
	// }
	Posting(int d, int t): did(d), tid(t){
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
	std::vector<Posting> global_buffer;
	int file_counter;
public:
	/*constructor*/
	Builder(const std::string index, const std::string lex, 
		const std::string doc_length, const int doc_num);
	~Builder();
	/*get the (did, tid, freq) tuples*/
	void GetPostings(const uint term_id);
	/*generate postings from term_id a to term_id b*/
	void GeneratePostings(const uint start, const uint end);
	/*write buffer to disk*/
	void WriteToDisk();
};

#endif