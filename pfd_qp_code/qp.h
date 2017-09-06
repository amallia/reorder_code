#include "globals.h"
#include "ListIterator.h"
#include "PostingOriented_BMW.h"
#include <bitset>

#ifdef CPP0X
	typedef std::unordered_map<std::string, float> strToFloatMap;
#else
	typedef std::map<std::string, float> strToFloatMap;
#endif

class QueryLogManager {
	std::vector<std::vector<std::string> > queriesD;
	strToFloatMap mapQueryToScore;
	termsMap *lex;

public:
	class queriesFileIterator : public std::iterator<std::forward_iterator_tag, std::vector<std::string> > 	{
		const QueryLogManager& queriesP;
		int curr;
		size_t count;
		size_t limit;
		size_t bucket;

		bool isSkip() const;
	public:
	  queriesFileIterator(const QueryLogManager& lm, int cur=-1) :queriesP(lm), curr(cur), count(0), limit(0), bucket(0) {}
	  queriesFileIterator(const queriesFileIterator& it) : queriesP(it.queriesP), curr(it.curr){}
	  bool operator==(const queriesFileIterator& rhs) {return curr==rhs.curr;}
	  bool operator!=(const queriesFileIterator& rhs) {return curr!=rhs.curr;}
	  const std::vector<std::string>& operator*() {return queriesP[curr];}
	  float score() const { return curr<queriesP.size() ? queriesP.score(queriesP[curr]) : -1.0; }
	  void changeSkipPolicy(size_t lim, size_t buck) { limit=lim; bucket=buck;}
	  queriesFileIterator& operator++();
	  int Getqid(){return queriesP.qidD[curr];}
	};
	
	std::vector<int> qidD;
	QueryLogManager(const char* fname, termsMap *l); //will add more loaders in future?
	void setScoreForQuery(const std::vector<std::string>& terms, float v);
	size_t size() const { return queriesD.size(); }
	const queriesFileIterator end() { return queriesFileIterator(*this,queriesD.size()); }
	const std::vector<std::string>& operator[] (size_t q) const { return queriesD[q]; }
	float score(const std::vector<std::string>& terms) const;
	void loadScoresFromFile(const char* fname);
};


void dumpResult(int qn, int rlen, const std::vector<std::string>& word_l, QpResult* res, FILE* fresult_log, FILE* fqscore );

class QueryProcessing {
public:
	int qn;
	termsCache& Cache;
	termsMap termsInCache;
	termsMap mappingForOnDemand;
	unsigned int* pages;
	unsigned int* loadDocLen(const char* fname=CONSTS::doclenFileName.c_str());
	lptrArray openLists(const std::vector<std::string>& word_l, termsMap& lex);
	QueryProcessing(termsCache& cache);
	void fillTermsMap(termsMap& lex, const std::string&);
	void operator()(const char* queryLog, const int buckets, const int limit, const int topk, const int layer);
	void printReport() const;
};
