#ifndef GOV2_SCANNER
#define GOV2_SCANNER
#include <iostream>
#include <vector>

namespace ScannerData{
	const std::string kInputFile = "/home/qw376/reorder_data/tmp_buffer_data_gov2/merged_index";
	const std::string kOutputIndex = "/home/qw376/reorder_data/compressed_doc_index/gov2_doc_index";
	const std::string kOutputLex = "/home/qw376/reorder_data/compressed_doc_index/gov2_doc_lex";
	const std::string kVBIndex = "/home/qw376/reorder_data/compressed_doc_index/vb_gov2_doc_index";
	const std::string kVBLex = "/home/qw376/reorder_data/compressed_doc_index/vb_gov2_doc_lex";
	const std::string kRawIndex = "/home/qw376/reorder_data/compressed_doc_index/raw_gov2_doc_index";
	const std::string kRawLex = "/home/qw376/reorder_data/compressed_doc_index/raw_gov2_doc_lex";
	const long long kTotalPostings = 6279431929;
	const int kIntSize = 4;
	const uint kThreshold = 50000000;   //previous version crashed here, 7982570901
	const uint kNumGov2Docs = 25205179;
}

class Scanner{
private:
	FILE* fInput;
	std::vector<uint> tids;
	long long num_of_postings;
	int pre_did;
	int pre_tid;
	long long offset;
    std::vector<unsigned int> dids;
    std::vector<unsigned int> lengths;
    std::vector<long long> offsets;
   	std::vector<uint> list_raw;

public:
	Scanner(const std::string input_file);
	~Scanner();
	void ScanRaw();
	void WriteIndexRaw(std::string lex_path, std::string index_path);
};

class ScannerVB{
private:
	FILE* fInput;
	std::vector<uint> tids;
	long long num_of_postings;
	int pre_did;
	int pre_tid;
	long long offset;
    std::vector<unsigned int> dids;
    std::vector<unsigned int> lengths;
    std::vector<unsigned int> c_sizes;
    std::vector<long long> offsets;
   	std::vector<unsigned char> compressed_list;

public:
	ScannerVB(const std::string input_file);
	~ScannerVB();
	void ScanVB();
	uint compressionVbytes(std::vector<uint> input);
	void WriteIndex(std::string lex_path, std::string index_path);
};

/*for test purpose*/
class ReaderVB{
public:
	void LoadLexVB(const std::string lex_path,
	uint* lengths, uint* c_sizes, long long* offsets);
	void ReadDocumentVB(uint did, const std::string index_path,
	uint* lengths, uint* c_sizes, long long* offsets);
};

class Reader{
public:
	void LoadLexRaw(const std::string lex_path,
	uint* lengths, long long* offsets);
	void ReadDocumentRaw(uint did, const std::string index_path,
	uint* lengths, long long* offsets);
};

#endif