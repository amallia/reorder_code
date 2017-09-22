#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <stdlib.h>
#include <assert.h>
#include <bitset>
#include <map>
#include <climits>
#include <sstream>
#include <iomanip>
#include <set>
#include <queue>
#include <sstream>
#include <unordered_set>

#include "Lsh.h"

using namespace std;

int decompressionVbytesInt(unsigned char* input, unsigned * output, int size){
    unsigned char* curr_byte = input;
    unsigned n;
    for (int i = 0; i < size; ++i) {
      unsigned char b = *curr_byte;
      n = b & 0x7F;
//        cout<<"The first byte: "<<n<<endl;
//        print_binary(n);
//        cout<<endl;

      while((b & 0x80) !=0){
        n = n << 7;
        ++curr_byte;
          b = *curr_byte;
          n |= (b & 0x7F);
//          cout<<"The following byte: "<<n<<endl;
//          print_binary(n);
//          cout<<endl;
      }
    ++curr_byte;
    output[i] = n;
  }

  int num_bytes_consumed = (curr_byte - input);
  return (num_bytes_consumed >> 2) + ((num_bytes_consumed & 3) != 0 ? 1 : 0);
}

void docGenerator::loadDocLexTSP(const uint numOfDocsToLoad) {

    // Lexicon.
    this->docLexFP = fopen(CONST::lexFileTSPQi.c_str(), "r");
    if (this->docLexFP == NULL) {
       cout << "Could not open file: " << CONST::lexFileTSPQi << ". Exiting." << endl;
       exit(0);
    }
      
    assert(numOfDocsToLoad > 0 && numOfDocsToLoad <= CONST::numDocs);
    vector<docLexTSP> dlex;
    
    // Temporary vars for fscanf.
    vecUlong tmpVec(CONST::docIndexLexElems, 0);
    char baseUrl[ 10240 ];
    char fullOrSuffixUrl[ 10240 ];
    char docidAr[ 1024 ];
    unsigned long docLen = 0;
    unsigned int docsLoaded = 0;
    // Document doc;
    
    while (fscanf( this->docLexFP, "%s%lu%lu%lu%lu%s%s\n",
        docidAr, &tmpVec[0], &tmpVec[1], &tmpVec[2], &docLen, baseUrl, fullOrSuffixUrl) != EOF) {
        string docid(docidAr);
        cmpList tmpList(docid, StrToUint(docid), (unsigned int)tmpVec[0], 0, tmpVec[1], tmpVec[2]);
        struct docLexTSP dlexentry;
        dlexentry.cmList = tmpList;
        dlexentry.docId = StrToUint(docid);
        dlexentry.docLen = docLen;
        dlexentry.url = fullOrSuffixUrl;
        dlexentry.baseUrl = baseUrl;
        this->dLex.push_back(dlexentry);
        ++docsLoaded;
        if (docsLoaded == numOfDocsToLoad) break; // Termination condition.
    }
    // Reporting.
    cout << "# lexicon entries loaded: " << docsLoaded << endl;
    fclose(this->docLexFP);
    // fclose(this->docIndexFP);
}


Document docGenerator::getNextDocument(struct docLexTSP dlexEntry) {
  Document curDoc;
    
  if (!dlexEntry.cmList.LoadFromIndex(*this->docIndexFP)) {
    this->docIndexFP->close();
    cout << "Could not load the index for document: " << dlexEntry.docId
         << ", so we return." << endl;
    exit(0);
  }
    
  // Decompress the termIds and frequencies.
  dlexEntry.cmList.DecompressVarByteHighWithoutPrefixSum();
    
  // Create the terms object.
  vector<Term> Terms (dlexEntry.cmList.len, Term());
  for (size_t i = 0; i < dlexEntry.cmList.len; ++i) {
     Terms[i].termId = dlexEntry.cmList.tids[i];
     Terms[i].frequency = dlexEntry.cmList.freqs[i];
        
     assert(Terms[i].termId >= 0);
     assert(Terms[i].frequency > 0);
  }
  curDoc.setTermVec(Terms);
  curDoc.setUrl(dlexEntry.url);
  curDoc.setDocId(dlexEntry.docId);
  curDoc.setBaseUrl(dlexEntry.baseUrl);
  curDoc.setDocLength(dlexEntry.docLen);
  curDoc.setTermCount(Terms.size());
    
  return curDoc;
}

Document docGenerator::getNextDocumentSeq(struct docLexTSP dlexEntry) {
  Document curDoc;
    
  if (!dlexEntry.cmList.LoadFromIndexSequentially(*this->docIndexFP)) {
    cout << "Could not load the index for document: " << dlexEntry.docId
         << ", so we return." << endl;
    exit(0);
  }
    
  // Decompress the termIds and frequencies.
  dlexEntry.cmList.DecompressVarByteHighWithoutPrefixSum();
    
  // Create the terms object.
  vector<Term> Terms (dlexEntry.cmList.len, Term());
  for (size_t i = 0; i < dlexEntry.cmList.len; ++i) {
     Terms[i].termId = dlexEntry.cmList.tids[i];
     Terms[i].frequency = dlexEntry.cmList.freqs[i];
        
     assert(Terms[i].termId >= 0);
     assert(Terms[i].frequency > 0);
  }
  curDoc.setTermVec(Terms);
  curDoc.setUrl(dlexEntry.url);
  curDoc.setDocId(dlexEntry.docId);
  curDoc.setBaseUrl(dlexEntry.baseUrl);
  curDoc.setDocLength(dlexEntry.docLen);
  curDoc.setTermCount(Terms.size());
    
  return curDoc;
}

int docGenerator::compressionVbytes(vector<uint> input){
   int compressedSize = 0;
   for (int i = 0; i < input.size(); ++i){
          unsigned char byteArr[kIntSize];
          bool started = false;
          int x = 0;

          for (x = 0; x < kIntSize; x++) {
             byteArr[x] = (input[i]%128);
             input[i] /= 128;
          }

          for (x = kIntSize - 1; x > 0; x--) {
            if (byteArr[x] != 0 || started == true) {
              started = true;
              byteArr[x] |= 128;
              this->compressedList.push_back(byteArr[x]);
              compressedSize++;
            }
          }
          this->compressedList.push_back(byteArr[0]);
          compressedSize++;
   }
   return compressedSize;
}

void docGenerator::generateDocIndex(){
  this->loadDocLexTSP(kNumberOfDocs);
  //Open Index File.
  this->docIndexFP = new ifstream(CONST::indexFileTSPQi.c_str(), ios::binary);  // Note we read in binary mode.
  if (!this->docIndexFP) {
    cout << "Could not open index file: " << CONST::indexFileTSPQi << " so we return." << endl;
    this->docIndexFP->close();
    exit(0);
  }
  Document currentDoc;
  unsigned long newOffset = 0;
  for (uint i = 0; i < kNumberOfDocs; i++) {
  // for (uint i = 0; i < 10; i++) {
    currentDoc = getNextDocument( this->dLex[i] );
    // currentDoc = getNextDocumentSeq( this->dLex[i] );
    const vector<Term> & terms = currentDoc.getTerms(); 
    vector<uint> tids;
    for (uint i=0; i< terms.size(); i++) {
      tids.push_back(terms[i].termId);
    }
    sort(tids.begin(), tids.end());

    // for(uint n = 0; n < tids.size(); n++){
    //   cout << tids[n] << " ";
    // }
    // cout << endl;

    for(uint n = tids.size() - 1; n >=1; n--){
      tids[n] = tids[n] - tids[n-1];
    }
    uint compressedSizeDelta = compressionVbytes(tids);
    cout << i << " " << tids.size() << " " << compressedSizeDelta << " " << newOffset << endl;
    dids.push_back(i);
    lengths.push_back(tids.size());
    compressedSizes.push_back(compressedSizeDelta);
    offsets.push_back(newOffset);
    newOffset += compressedSizeDelta;
  }
  this->WriteIndex(kCompressedDocLex, kCompressedDocIndex);
  //close index file
  this->docIndexFP->close();
}


void docGenerator::WriteIndex(string lex_path, string index_path){
  /*dump buffer to file*/
  dumpToFile(index_path, this->compressedList.begin(), this->compressedList.end());
  /*dump lexicon to file*/
  FILE * lexDir = fopen(lex_path.c_str(), "w");
  if( lexDir == NULL){
     cout << "Problem! The file: " << lex_path << " could not be opened!" << endl;
     exit(0);
  }
  for(int i = 0; i < dids.size(); ++i){
     fprintf(lexDir, "%u %u %u %lu\n", dids[i], lengths[i], compressedSizes[i], offsets[i]);
  }
  fclose(lexDir);
    cout << "compressed doc index generated" << endl;
}
