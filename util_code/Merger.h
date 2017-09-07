#include <vector>
#include <string>
#include <iostream>
#include <assert.h>
#include <unordered_map>
#include <sstream>
#include <map>
using namespace std;

template <typename Iterator>
void dumpToFile(const std::string& path, Iterator start, Iterator end) { ///home/qw376/SIGIR2017/data
  // FILE *fdo = fopen64(path.c_str(),"w");
  FILE *fdo = fopen(path.c_str(),"a+");
  if(! fdo) {
    cout << "Failed writing to " << path << " (Hint: create folder for path?)" << endl;
    return;
  }
  assert(fdo);
  for (; start !=end; ++start)
    if (fwrite(&(*start), sizeof(typename Iterator::value_type), 1, fdo) != 1)
      cout << "Failed writing to " << path << " (Hint: create folder for path?)" << endl;
  fclose(fdo);
}

struct docLexTSP {
  uint docId;
  uint docLen;
  // cmpList cmList;
  string url;
  string baseUrl;
};

struct urlDocId {
    string url;
    unsigned int docId;
    
};

class LexInfo{
public:
	uint termId;
	uint listLen;
	uint listLenInBytes;
	unsigned long offset;

	LexInfo(uint a, uint b, uint c, unsigned long d){
		termId = a;
		listLen = b;
		listLenInBytes = c;
		offset = d;
	}
};

struct comparator {
    bool operator()(struct urlDocId a, struct urlDocId b){
        return (a.url.compare(b.url) < 0) ? true : false;
    }
};

class Merger{
private:
	const string graphIndex = "/home/qw376/SIGIR2017/graphIndex/graph";
	const string graphLex = "/home/qw376/SIGIR2017/graphIndex/graphLex";
	const string deltaGraphIndex = "/home/qw376/SIGIR2017/graphIndex/deltaGraphIndex100_411";
	const string deltaGraphLex = "/home/qw376/SIGIR2017/graphIndex/deltaGraphLex100_411";
	const string mergedGraphIndex = "/home/qw376/SIGIR2017/graphIndex/mergedGraphIndex100_411";
	const string mergedGraphLex = "/home/qw376/SIGIR2017/graphIndex/mergedGraphLex100_411";
	// const string deltaGraphIndexOld = "/home/qw376/SIGIR2017/graphIndex/deltaGraphIndex87";
	// const string deltaGraphLexOld = "/home/qw376/SIGIR2017/graphIndex/deltaGraphLex87";  
	const string deltaGraphIndexOld = "/home/qw376/SIGIR2017/graphIndex/graphTest312";
	const string deltaGraphLexOld = "/home/qw376/SIGIR2017/graphIndex/graphLexTest312"; 
	const unsigned int intSize = 4;

	//lexicon and buffer for the graph
	unsigned long offset;
	vector<unsigned int> didsGraph;
	vector<unsigned int> compressedSizesGraph;
	vector<unsigned int> lengthsGraph;
	vector<unsigned long> offsetsGraph;
	vector<unsigned int> impactGraph;
	vector<unsigned char> compressedList;

	// ifstream* docIndexFP;
	FILE* docLexFP;
	const string lexFileTSPQi = "/home/qw376/WWW2017/data/docIndex/lexWithBaseUrlFullUrl_Random_Seq";//../data/lex_full";
  	const string indexFileTSPQi = "/home/qw376/WWW2017/data/docIndex/DocIndexSeq";//../data/index_full";
  	vector<struct urlDocId> urls;
  	unsigned int docIndexLexElems = 3;
  	vector<vector<unsigned int>> urlNeighborArray;
  	vector<vector<unsigned int>> lshNeighborArray;
  	const unsigned int numDocs = 50219612;
  	const unsigned int urlRange = 50;

  	//remap the ordering
  	const string randomOrder = "/home/qw376/SIGIR2017/didReassignment/randomOrder";
  	const string tspOrder = "/home/qw376/SIGIR2017/didReassignment/tspOrder318";
  	const string newOrder = "/home/qw376/SIGIR2017/didReassignment/tspOrder323";

  	map<unsigned int, unsigned int> randomOrderMap;
  	map<unsigned int, unsigned int> originalOrderMap;

  	//query generation
  	// const string rawQuery = "/home/qw376/SIGIR2017/testQueries/1kRawQueries";
  	const string rawQuery = "/home/qw376/SIGIR2017/testQueries/toyQueries408";
  	const string numQuery = "/home/qw376/SIGIR2017/testQueries/numQuery";
  	const string queryLex = "/home/qw376/SIGIR2017/testQueries/queryLex";
  	const string wholeLex = "/home/qw376/Info_Clueweb/lexikon.txt";
  	unordered_map<string, LexInfo> lexMap;

  	//query selection
  	const string queryPool = "/home/qw376/SIGIR2017/testQueries/queryPool";

  	//doc length
	const int MAXD = 50219612;
  	unsigned int* doclen;
  	unordered_map<unsigned int, unsigned int> newOrderMap;
  	// const string ordering = "/home/qw376/SIGIR2017/didReassignment/randomOrder";
  	// const string ordering = "/home/qw376/SIGIR2017/didReassignment/urlOrder";
  	const string ordering = "/home/qw376/SIGIR2017/didReassignment/tspOrder323";
  	const string docLenFileDir = "/home/qw376/Info_Clueweb/documentLengths"; //doc lengths

public:
	void generateDeltaGraph();
	void writeDeltaIndex();
	void writeMergedIndex();
	int compressionVbytes(vector<uint> input);
	void loadDeltaGraph();
	void verifyGraph();
	void mergeGraph();
	void loadUrls();
	void getUrlNeighbors();
	void mergeNeighbors();

	//order remapping
	void orderReMap();
	void loadRandomOrder();
	void loadOriginalOrder();
	void getNewOrder();

	//query
	void queryGenerator();
	void buildSmallLex();
	void loadLex();
	void buildTermIDQueryLog();

	//1k query selection
	void queryTermIDSelector();
	void rawQuerySelector();

	//docLength remap
	void loadDocLength();
	void loadNewOrdering();
	void writeOutDocLengthFile();
	void docLengthRemap();

	//url Graph
	void buildURLOnlyGraph();
};