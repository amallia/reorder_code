#ifndef __docroutelocal__Lsh__
#define __docroutelocal__Lsh__

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <stdlib.h>
#include <assert.h>
#include <bitset>
#include <map>
#include <unordered_map>
#include <climits>
#include <sstream>
#include <iomanip>
#include <set>
#include <queue>
#include <thread>

using namespace std;

typedef unsigned int uint;

// Parameters for number of candidates.
const uint pairsThreshold = 100;
const uint maxCandidates = 300; // before filtering.
const uint numCandidates = 100; // after filtering.

class listTSP{
public:
  // uint termID;
  uint startDid;
  uint endDid;
  listTSP(uint startDocId) {
    this->startDid = startDocId;
    this->endDid = startDocId;
  }
  // listTSP(uint startDocId, uint endDocId){
  //   this->startDid = startDocId;
  //   this->endDid = endDocId;
  // }
  void setEndDid(uint did){
    this->endDid = did;
  }
};

class Edge {
  public:
   uint _docID;
   double _similarity;
   // Constructors.
   Edge() {
     _similarity = 0.0f;
     _docID = 0;
   }
   Edge(const uint inDocID, const double inSimilarity) {
     _docID = inDocID;
     _similarity = inSimilarity;
   }
};

class edgeComparatorDid {
  public:
    bool operator() (const Edge& a, const Edge& b) const {
      return (a._docID == b._docID);
    }  
};

class edgeComparator {
  public:
    bool operator() (const Edge& a, const Edge& b) const {
      return (a._similarity > b._similarity);
    }  
};

class edgeComparatorLargestFirst {
  public:
    bool operator() (const Edge& a, const Edge& b) const {
      return (a._similarity < b._similarity);
    }  
};

class Node {
  public:
   // Candidate edges before filtering (by computing full score).
   vector<Edge> _candidates;

   // Interface for adding/removing edges in Node.
   // Largest/(Smallest) similarity edge at the top.
   priority_queue<Edge, vector<Edge>, edgeComparatorLargestFirst> _edgeHeap;  

   // Add edge in the priority queue.
   void addEdge(const Edge& edge) {
     _edgeHeap.push(edge); // push edge
   }   

   // Constructor.
   Node() {}
};

class probPair{
public:
  uint tid;
  // double exp;
  float exp;
  probPair(uint t, float e){
    tid = t;
    exp = e;
  }
};

template <typename Iterator>
void dumpToFile(const std::string& path, Iterator start, Iterator end) { ///home/qw376/SIGIR2017/data
  FILE *fdo = fopen64(path.c_str(),"w");
  // FILE *fdo = fopen(path.c_str(),"a+");
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

template <typename Iterator>
void dumpToFileApp(const std::string& path, Iterator start, Iterator end) { ///home/qw376/SIGIR2017/data
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
/*******for reordering******/

/** tsp process **/
class docNode{
public:
  uint length;
  // vector<unsigned char> data;
  unsigned char * compressedList;
  // docNode(vector<unsigned char>& input, uint l){
  //   this->data = input;
  //   this->length = l;
  // }
  docNode(unsigned char * input, uint l){
    this->compressedList = input;
    this->length = l;
  }
};

class tspNode{
public:
  int impact;
  vector<unsigned> neighbors;
  tspNode(vector<unsigned>& inputVector, int score){
    this->neighbors = inputVector;
    this->impact = score;
  }
};

class tspRunner{
public:
  vector <tspNode> nodes;
  int numNodesSelected;
  const unsigned numOfDocs = 50219612;
  const unsigned numOfTerms = 86532822;
  // const unsigned numOfDocs = 10;
  const unsigned numOfTSPNodes = 10000;
  const uint numOfDocsGraph = 10;
  const uint sampleSize = 100;
  // const unsigned numOfNodes = 5;
  const uint probSampleLB = 50;
  const uint probSampleUB = 100;
  ifstream* docIndexFP;
  FILE* docLexFP;
  vector<uint> plLength;
  unordered_map<unsigned int, listTSP*> postingsListMap;
  unordered_map<unsigned int, listTSP*>::iterator pl_it;
  uint totalTerms;

  unordered_map<uint, vector<probPair> > probMap; 
  unordered_map<uint, vector<probPair> >::iterator it;
  unordered_map<uint, double> probMassMap;
  unordered_map<uint, double>::iterator iter;

  tspRunner(){
    cout << "tsp starts" << endl;
  }
  void run();
  void tsp();
  void updateTermLists(uint newDid, uint did);
  bool getNextCandidate(uint newDid, uint did);
  // void updateEdgeWeightShuai(uint newDid, uint did);
  void loadSortedPairs();
  void loadProbMass();
  
  //lexicon and buffer for the unique docs
  const string uniqueDocLex = "/home/qw376/SIGIR2017/docIndex/lex16";
  const string uniqueDocIndex = "/home/qw376/SIGIR2017/docIndex/index16";
  vector <docNode> compressedDocs;
  void loadCompressedDocs();
  void getDocFromMM(vector<uint> & terms, uint did);

  /*get dgap compressed graph index*/
  // const string deltaGraphLex = "/home/qw376/SIGIR2017/graphIndex/deltaGraphLex87";
  // const string deltaGraphIndex = "/home/qw376/SIGIR2017/graphIndex/deltaGraphIndex87";
  // const string deltaGraphIndex = "/home/qw376/SIGIR2017/graphIndex/mergedGraph317";
  // const string deltaGraphLex = "/home/qw376/SIGIR2017/graphIndex/mergedGraphLex317";
  const string deltaGraphIndex = "/home/qw376/SIGIR2017/graphIndex/mergedGraphIndex100_411";
  const string deltaGraphLex = "/home/qw376/SIGIR2017/graphIndex/mergedGraphLex100_411";
  vector <docNode> compressedGraphNodes;
  void loadCompressedGraph();
  void getNeighborsFromMM(vector<uint> & dids, uint did);

  /*For LM directly access */
  const string sortedPairs = "/home/qw376/WWW2017/newData/sortedPairsTids";
  const string probMass = "/home/qw376/WWW2017/newData/probPortion50Map";
  vector<float> probMassVector;
  vector< vector<probPair> > proVector;
  // unordered_map<unsigned int, listTSP*> postingsListMap;
  // unordered_map<unsigned int, listTSP*>::iterator pl_it;
  vector<uint> endDidVector;
  vector<bool> visited;
  uint candidate;
  void loadSortedPairsDA();
  void loadProbMassDA();

  /*output did assignment*/
  const string didMapping = "/home/qw376/SIGIR2017/didReassignment/tspOrder318";
  uint probPairsThreshold = 25;
  uint probPairsThresholdUp = 50;
  uint lastUnvisitedHasNeighbors = 0;
  uint lastUnvisitedHasNoNeighbors = 0;

};
int decompressionVbytesInt(unsigned char* input, unsigned * output, int size);
/** tsp process **/
#endif
