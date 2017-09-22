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

#include "globals.h"
#include "iterator.h"
#include "Document.h"
#include "Term.h"

using namespace std;

typedef unsigned int uint;

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

class tspNode{
public:
  int impact;
  vector<unsigned> neighbors;
  tspNode(vector<unsigned>& inputVector, int score){
    this->neighbors = inputVector;
    this->impact = score;
  }
};

struct docLexTSP {
  uint docId;
  uint docLen;
  cmpList cmList;
  string url;
  string baseUrl;
};

const uint kNumberOfDocs = 50219612;
const unsigned int kIntSize = 4;
const string kCompressedDocLex = "/home/qw376/reorder_data/compressed_doc_index/doc_lex";
const string kCompressedDocIndex = "/home/qw376/reorder_data/compressed_doc_index/doc_index";

class docGenerator{
public:
    ifstream* docIndexFP;
    FILE* docLexFP;
    vector<docLexTSP> dLex;
    const string uniqueDocLex = "/home/qw376/SIGIR2017/docIndex/lex16";
    const string uniqueDocIndex = "/home/qw376/SIGIR2017/docIndex/index16";

    unsigned long offset;
    vector<unsigned int> dids;
    vector<unsigned int> compressedSizes;
    vector<unsigned int> lengths;
    vector<unsigned long> offsets;
    vector<unsigned char> compressedList;

    docGenerator(){
      cout << "doc generation" << endl;
    }

    void generateDocIndex();
    void loadDocLexTSP(const uint numOfDocsToLoad);
    Document getNextDocument(struct docLexTSP dlexEntry);
    Document getNextDocumentSeq(struct docLexTSP dlexEntry);
    int compressionVbytes(vector<uint> input);
    void WriteIndex(string lex_path, string index_path);
};

int decompressionVbytesInt(unsigned char* input, unsigned * output, int size);
/** tsp process **/
#endif
