#include <vector>
#include <string>
#include <iostream>
#include <assert.h>
#include <unordered_map>
#include <sstream>
#include <map>
#include  <random>
#include  <iterator>
using namespace std;

template<typename Iter, typename RandomGenerator>
Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
    std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
    std::advance(start, dis(g));
    return start;
}

template<typename Iter>
Iter select_randomly(Iter start, Iter end) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return select_randomly(start, end, gen);
}

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

const string kUrlLex = "/home/qw376/reorder_data/graph_index/url_lex";
const string kUrlIndex = "/home/qw376/reorder_data/graph_index/url_index";

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
	const int intSize = 4;

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
	void mergeGraph();
	void loadUrls();
	void getUrlNeighbors();
	void mergeNeighbors();

	//order remapping
	void orderReMap();
	void loadRandomOrder();
	void loadOriginalOrder();
	void getNewOrder();

	//docLength remap
	void loadDocLength();
	void loadNewOrdering();
	void writeOutDocLengthFile();
	void docLengthRemap();

	//url Graph
	void BuildURLOnlyGraph();
	void CompressUrlGraph();
	void WriteIndex(string lex_path, string index_path);

	//for verification
	void verifyGraph(string lex_path, string index_path);
	void verifyDeltaGraph(string lex_path, string index_path);
};

/***************************************************************************
language model and query related stuff
***************************************************************************/

const string kWholeLex = "/home/qw376/Info_Clueweb/lexikon.txt";
// const string rawQuery = "/home/qw376/SIGIR2017/testQueries/1kRawQueries";
const string kRawQuery = "/home/qw376/SIGIR2017/testQueries/toyQueries408";
const string kNumQuery = "/home/qw376/SIGIR2017/testQueries/numQuery";
const string kQueryLex = "/home/qw376/SIGIR2017/testQueries/queryLex";
//query selection
const string kQueryPool = "/home/qw376/reorder_data/query/3kQueries";
const string kTwoTermsQuery = "/home/qw376/reorder_data/query/2terms3kQueries";
const string kRandomQuery = "/home/qw376/reorder_data/query/1kRandomQuery";
const string kSmallLex = "/home/qw376/reorder_data/lexicon/3kQueryLexicon";
const string kCommonTermFile = "/home/qw376/reorder_data/query/common_terms";
//LM
const string kIdealLMPath = "/home/qw376/reorder_data/LM/ideal_lm";
const string kLMWithTermID = "/home/qw376/reorder_data/LM/ideal_lm_termIDs";
const string kTermTable = "/home/constantinos/Datasets/GOV2/word_file";
//Gov2
const string kGov2WordTable = "/home/constantinos/Datasets/GOV2/word_file";
// const string kGov2Query = "/home/qw376/reorder_data/query/1kRandomQuery_1st";
const string kGov2Query = "/home/qw376/reorder_data/query/cherry_queries";
const string kGov2SmallLex = "/home/qw376/reorder_data/lexicon/gov2_small_lex_2nd";

class LMMaker{ 
	private:
		unordered_map<string, LexInfo> lexMap;
	public:
		//query
		void queryGenerator();
		void LoadLex();
		void buildTermIDQueryLog();

		//1k query selection
		// void queryTermIDSelector();
		// void rawQuerySelector();
		void RandomGetOneKQuery(const std::string input_query,
 		const std::string output_query);

		//two terms query generation
		void LoadSmallLex(const string input_lex);
		void GetTwoTermQueries(const std::string lex_file, const std::string input_query,
  		const std::string common_term_file, const std::string output_query);

		//build small lex
		void buildSmallLex(const string input_query, const string output_lex);

		//build ideal lm
		void GenerateIdealLM(const string query_trace, const string lm_path);
		void TransformLM(const string lex_path,
			const string lm_path, const string lm_path_termID);

};

class Gov2Utils{
private:
	unordered_map<string, int> lex_map;

public:	
	/*build small lex for gov2*/
	void LoadLex();
	void Gov2SmallLex(const string query_trace, const string lex);
};