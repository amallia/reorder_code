#ifndef docid_reordering_globals_h
#define docid_reordering_globals_h

#include <stdio.h>
#include <string>
#include <vector>
#include <math.h>

using namespace std;

typedef unsigned int uint;
typedef unsigned long ulong;
typedef vector<uint> vecUint;
typedef vector<ulong> vecUlong;
typedef vector<string> vecString;
typedef vector<unsigned char> vecUchar;
typedef vector<char> vecChar;
typedef vector<double> vecDouble;
typedef long int stat_t;

namespace CONST {
  // Configuration for experiments.
  // const int totalDocuments = 50219612;
  const int totalDocuments = 1000;
  const int numOfShards = 1; //const int numOfShards = 16;
  const uint numOfIndexBeforeMerge = 1; // const uint numOfIndexBeforeMerge = 3; // 5000; // 3; //5000
  const uint numOfDocsToRoute = totalDocuments; // 200000;//const uint numOfDocsToRoute = 24000000; // 200000;
  const uint bufferThreshold = totalDocuments;//const uint bufferThreshold = 100000; //100000000; //100000;
  const uint samplingrate = totalDocuments; //20000;
  const uint dynamicPolicy = 0; // 0 = Docs, 1 = Index.
  const uint docIDReordering = 2; // 0 = No reordering during merging, 1 = URL reordering, 2 = LSH
  const unsigned long long int bufferSizeThreshold = totalDocuments; // TOCHECK

  const uint qprate = 40000; // TODO(dimopoulos): merge or not with sampling rate!! CURRRENTLY DISABLED
  const uint numOFDocsToLoadAndRoute = totalDocuments; //TODO used or not ?
  const size_t geqQuerySz = 1;  // greater equal query size for valid queries
  const size_t lessthanQuerySz = 12; // less than query size for valid queries
  const bool emptyrun = false;  // TOGO ?
  const uint sizeofSmallPL = 6;

  // Raw inverted index and lexicon.
  // Pangolin: "../../", Dodo: "../data/"
  const string rawPath = "/home/qw376/WWW2017/data/";
  const string rawIndex = "InvertedIndex";
  const string rawLexicon = "lexikon.txt";
  const size_t rawLexiconElems = 10;
  // Pangolin: /data/PositionIndex, Dodo: /research/maria/index files 3/PositionIndex
  const string posIndexFile = "/home/qw376/WWW2017/data/PositionIndex";
    
  // Index and lexicon files for construction.
  // Format: <tid, freq> sorted by tid, Reading with IterateDocIndex().
  const string lexFile = "/home/constantinos/OnlineDocumentRouting/data/fullDocIndexSequential/lexWithBaseUrlFullUrl_Random_Seq";//../data/lex_full";
  const string indexFile = "/home/constantinos/OnlineDocumentRouting/data/DocIndexSeq";//../data/index_full";

  const string lexFileTSPQi = "/home/qw376/WWW2017/data/docIndex/lexWithBaseUrlFullUrl_Random_Seq";//../data/lex_full";
  const string indexFileTSPQi = "/home/qw376/WWW2017/data/docIndex/DocIndexSeq";//../data/index_full";

  const string sortedPairs = "/home/qw376/WWW2017/newData/sortedPairsTids";
  const string probMass = "/home/qw376/WWW2017/newData/probPortion50Map";

  const string termLex = "/home/qw376/WWW2017/data/randomLexicon"; //this is not used here, all terms has been converted to tids for the input files
    
  // Format: <tid> sorted by position (as they appear in the actual document)
  // Reading with IterateFinalDocPosIndex().
  const string docLexFile = "/home/qw376/WWW2017/data/docLex";
  const string docIndexFile = "/home/qw376/WWW2017/data/docIndex";
    
  // Format: <tid, position> sorted by tid, Reading with IterateDocIndex()
  const string posDocLexFile = "/home/qw376/WWW2017/data/posDocLex";
  const string posDocIndexFile = "/home/qw376/WWW2017/data/posDocIndex";
    
  const size_t docIndexLexElems = 3; // len offset lenBytes
  const string docLengthFile = "/home/qw376/WWW2017/data/docLengths";
  const string lexWithUrlFile = "/home/qw376/WWW2017/data/lex_with_url";
  const string docToUrlFile = "/home/qw376/WWW2017/data/alexaFeatures";
    
  // Docid orderings
  const size_t numReorderSchemes = 2;
  const string randomDocidOrdering = "/home/qw376/WWW2017/data/randomDocidOrdering";
  const string urlSortedDocidOrdering = "/home/qw376/WWW2017/data/urlSortedDocidOrdering";
  const string docidUrlParsedUrl = "/home/qw376/WWW2017/data/docidUrlParsedUrl";

  //minHash store
  const string minHashDir = "/home/qw376/SIGIR2017/data/minHashIndex";
  const string minHashLexDir = "/home/qw376/SIGIR2017/data/lexMinHash";
  const string minHashReadDir = "/home/qw376/SIGIR2017/data/minHashIndexAll";
  const string minHashReadLexDir = "/home/qw376/SIGIR2017/data/lexMinHashAll";
   
  // Compression types.
  // TODO(dimopoulos): change to ENUM.
  // 0: Delta, 1: Gamma, 2: VB, 3: PFD Zero pad, 4: PFD MAXD pad,
  // 5: IPC, 6:OPT-PFD
  const size_t numCompressionTypes = 7;
    
  // Data processing files.
  const string dataPath = "/home/qw376/WWW2017/data/";
  const string lexicon = "/home/constantinos/OnlineDocumentRouting/data/lexicon"; //"../data/lexicon";
  const size_t lexElems = 5;
    
  // Various
  const unsigned int byteBits = 7;
  const size_t printNumDecoded = 5;
  const string docFeatures = "/home/qw376/WWW2017/data/docFeatures";
  const size_t docFeaturesElems = 5;

  // Compression
  const int numBytesInInt = 4;
  const uint pfdBlock = 64;
    
  // costmetic type TODO: what each of this method performs ?
  const unsigned int endOfListDgap = 0;
  const unsigned int deltaEncoding = 1;
  const unsigned int deltaEncodingExtended = 2;
  const unsigned int uniformProgression = 3;
  const unsigned int avgGreedy = 4;
  const unsigned int greedyWithFrequency = 5;
  const unsigned int bestPerformerGreedy = 6;
  const unsigned int rewardpunishEncoding = 7;
	
  // Query files.
  const string million_tids = "/home/qw376/WWW2017/data/queries/million09_1k_queries_final";
  const string million = "/home/qw376/WWW2017/data/queries/million09_1k_queries";
  const string trec = "/home/qw376/WWW2017/data/queries/trec06_1k_queries";
  const string trec_tids = "/home/qw376/WWW2017/data/queries/trec06_1k_queries_final";
  const bool useTrecQueries = true;

  // Url distribution.
  const string hostsWithSizesInUrlSortedOrder = "/home/qw376/WWW2017/data/hostsWithSizesInUrlSortedOrder";
  const string urlSampleFile = "/home/constantinos/OnlineDocumentRouting/data/urlSample";
  const uint hostSizeMean = 10;
  const uint urlSampleSize = 2500000;

  // Term -> TermId mapping file
  const string termIdMapping = "termIdMapping";
  const string termIDMappingFile = "/home/constantinos/OnlineDocumentRouting/data/termIdMapping";

  // Constants.
  const int topK = 10;  // k in top-k results returned to the user.
  const uint blockSize = 128;  // postings per block.
  const float epsilon = 0.00001;  // for double/float comparison.
  const float k1 = 1.2;
  const float b = 0.75;
  const uint freqStart = 5; // term-based approach: keep freqs 5 - 10^6 Lempel et al.
  const uint freqEnd = 1000000; 
  const string freqTermBased = "/home/constantinos/OnlineDocumentRouting/data/freqTermBased"; // freqs in range for the Term-based approach.
  const uint termBasedFreqs = 13344594;  // # entries in freqTermBased file is 13344594

  // Statistics
  const uint numTerms = 86532822;
  const uint numDocs = 50220423;
  const unsigned long long int numPostings = 17075485964;
  const float avgDocLen = 693.433f;
}

#endif
