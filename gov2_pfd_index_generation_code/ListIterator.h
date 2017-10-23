#ifndef LISTITERATOR_H_
#define LISTITERATOR_H_

#include <vector>
#include <algorithm>
#include <iterator>

#include "SqlProxy.h"
#include "pfor.h"
#include "globals.h"
// #include "profiling.h"
#include "sql/sqlite3.h"
#include "BlockGens.h"
using namespace std;

class BasicList {
public:
	std::string term;
	size_t termId;
	unsigned int lengthOfList;				// the padded list length
	unsigned int unpadded_list_length;		// needed only for layering (unpadded list length)
	unsigned int original_unpadded_list_length; // for layering, useless but we need to remove it from prepare_list - trecreader
	float maxScoreOfList; //optional
	BasicList(const std::string& t="", size_t id=0) : term(t), termId(id) {}
};

class RawIndexList : public BasicList{
public:
	vecUInt doc_ids;
	vecUInt freq_s;
	std::vector<float> scores; //optional
	explicit RawIndexList(const BasicList& rhs=BasicList()) :BasicList(rhs) { }
	void rankWithBM25(unsigned int* doclen); //will fill the scores
	void rankWithBM25(unsigned int* doclen, unsigned int& unpadded_list_length); // compute the correct max score of list
};

class CompressedList : public BasicList {
public:
	size_t BS;
	vecUInt maxDidPerBlock;		// the max value for each chunk
	std::vector<float> DEPmaxScorePerBlock;
	vecUInt sizePerBlock;		// the size for each chunk
	vecUInt offsetPerBlock;  // the offset for each chunk
	vecUInt flag;			// the flag for each chunk
	vecUInt compressedVector;	// the compressed data
	size_t cpool_leng;	// the length of cpool
	size_t info_leng;		// the length of info
	size_t numOfBlocks;

	CompressedList() :BS(0) {}
	CompressedList(const RawIndexList& rl, const size_t bs=CONSTS::BS);
	void compressDocsAndFreqs(const RawIndexList& rl);
	void serializeToFS(const std::string& rootPath);
	void serializeToDb(const std::string& rootPath, SqlProxy&);
};

template<typename T>
void injectBlocker(T& blocker, RawIndexList& rilist ) {
	T gen(rilist.doc_ids);
	gen.generateBlocks();
	gen.generateMaxes(rilist.scores);
	blocker = gen;
}

//Quantization
template<typename T>
void Quantized_injectBlocker(T& blocker, RawIndexList& rilist , float& quantile) {
	T gen(rilist.doc_ids);
	gen.generateBlocks();
	gen.generate_Quantized_Maxes(rilist.scores, quantile);
	blocker = gen;
}

template<typename T>
void clearBlocks(T& blocker) {
	blocker = T();
}


#endif /* LISTITERATOR_H_ */
