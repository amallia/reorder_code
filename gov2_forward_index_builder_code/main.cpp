#include "builder.h"
#include "merger.h"
#include "scanner.h"
#include <iostream>

int main(int argc, const char* argv[]){
	std::string option(argv[1]);
	/*get postings*/
	if(option == "-gp") {
		std::cout << "generating postings\n";
		Builder builder(BuilderData::kIndexPath, BuilderData::kLexPath, BuilderData::kDocLengths, BuilderData::kDocNum);
		std::vector<Posting> postings;
		builder.GeneratePostings(1, BuilderData::kTermNum);
	}

	/*merge postings*/
	if(option == "-mp") {
		std::cout << "merge postings\n";
		Merger merger(MergerData::kInputList);
		merger.merge();
	}

	/*get raw forward index with lex*/
	if(option == "-sp") {
		std::cout << "scan postings\n";
		Scanner scanner(ScannerData::kInputFile);
		scanner.ScanRaw();
	}

	/*get pfd version forward index with lex*/
	if(option == "-spvb") {
		std::cout << "scan postings then contruct vbyte index\n";
		ScannerVB scanner(ScannerData::kInputFile);
		scanner.ScanVB();
	}

	/*test reading documents*/
	if(option == "-read") {
		std::cout << "read a document\n";
		// std::vector<unsigned char> test;
		// std::cout << test.max_size() << std::endl;
		Reader r;
		uint* lengths = new uint[ScannerData::kNumGov2Docs];
		long long* offsets = new long long[ScannerData::kNumGov2Docs];
		r.LoadLexRaw(ScannerData::kRawLex, lengths, offsets);
		// uint did = ScannerData::kNumGov2Docs - 1;
		uint did = 0;
		// std::cout << lengths[25182428] << std::endl;
		r.ReadDocumentRaw(did, ScannerData::kRawIndex, lengths, offsets);
		delete[] lengths;
		delete[] offsets;
	}

	/*test reading documents compressed in VB*/
	if(option == "-readvb") {
		std::cout << "read a document compressed in VBytes\n";
		ReaderVB r;
		uint* lengths = new uint[ScannerData::kNumGov2Docs];
		uint* c_sizes = new uint[ScannerData::kNumGov2Docs];
		long long* offsets = new long long[ScannerData::kNumGov2Docs];
		r.LoadLexVB(ScannerData::kVBLex, lengths, c_sizes, offsets);
		// uint did = ScannerData::kNumGov2Docs - 1;
		uint did = 5774;
		// std::cout << lengths[25182428] << std::endl;
		r.ReadDocumentVB(did, ScannerData::kVBIndex, lengths, c_sizes, offsets);
		delete[] lengths;
		delete[] offsets;
	}
	return 0;
}