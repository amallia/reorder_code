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

typedef unsigned int uint;

int decompressionVbytes(unsigned char* input, unsigned short* output, int size){
    unsigned char* curr_byte = input;
    unsigned short n;
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

void tspRunner::run(){
  this->loadSortedPairsDA();
  this->loadProbMassDA();
  this->loadCompressedGraph();
  this->loadCompressedDocs();
  this->tsp();
}

void tspRunner::loadCompressedDocs(){
  cout << "Loading Compressed Docs\n";
  uint did;
  uint systemDid;
  uint length;
  uint compressedSize;
  unsigned long offset;

  FILE * lexFile;
  lexFile = fopen(this->uniqueDocLex.c_str(), "r");
  if( lexFile == NULL){
    cout << "Problem! The file: " << this->uniqueDocLex << " could not be opened!" << endl;
    exit(0);
  }

  FILE* IndexFile = fopen(this->uniqueDocIndex.c_str(),"r");
  if( IndexFile == NULL) {
    cout << "Problem! The file: " << this->uniqueDocIndex << " could not be opened!" << endl;
    exit(0);
  }

  for(uint i = 0; i <= this->numOfDocs; i++){
    unsigned char * p = NULL;
    docNode t(p, 0);
    this->compressedDocs.push_back(t);
  }

  while(fscanf(lexFile, "%u %u %u %u %lu\n", &did, &systemDid, &compressedSize, &length, &offset)!=EOF){
    // cout << did << " " << length << " " << compressedSize << " " << offset << endl;
    unsigned char * cp = new unsigned char[compressedSize];
    fread(cp, 1, compressedSize, IndexFile);
    docNode t(cp, length);
    this->compressedDocs[did] = t;
    // if(did == 1){
    //   break;
    // }
  }

  cout << "Compressed Docs Loading Done\n";
  fclose(lexFile);
  fclose(IndexFile);
}

void tspRunner::loadCompressedGraph(){
  cout << "Loading Compressded Graphs\n";
  uint did;
  uint length;
  uint compressedSize;
  unsigned long offset;

  FILE * lexFile;
  lexFile = fopen(this->deltaGraphLex.c_str(), "r");
  if( lexFile == NULL){
    cout << "Problem! The file: " << this->deltaGraphLex << " could not be opened!" << endl;
    exit(0);
  }

  FILE* IndexFile = fopen(this->deltaGraphIndex.c_str(),"r");
  if( IndexFile == NULL) {
    cout << "Problem! The file: " << this->deltaGraphIndex << " could not be opened!" << endl;
    exit(0);
  }

  for(uint i = 0; i <= this->numOfDocs; i++){
    unsigned char * p = NULL;
    docNode t(p, 0);
    this->compressedGraphNodes.push_back(t);
  }

  while(fscanf(lexFile, "%u %u %u %lu\n", &did, &length, &compressedSize, &offset)!=EOF){
    // cout << did << " " << length << " " << compressedSize << " " << offset << endl;
    unsigned char * cp = new unsigned char[compressedSize];
    fread(cp, 1, compressedSize, IndexFile);
    docNode t(cp, length);
    this->compressedGraphNodes[did] = t;
    // if(did == 1){
    //   break;
    // }
  }

  cout << "Compressed Graph Loading Done\n";
  fclose(lexFile);
  fclose(IndexFile);

}

void tspRunner::getDocFromMM(vector<uint> & terms, uint did){
  docNode d = this->compressedDocs[did];
  uint * uncompressedList = new unsigned[d.length];
  decompressionVbytesInt(d.compressedList, uncompressedList, d.length);
  terms.push_back(uncompressedList[0]);
  // cout << uncompressedList[0]<<" ";
  for(uint i = 1; i < d.length; ++i){
    // cout << uncompressedList[i]<<" ";
    terms.push_back( terms.back() + uncompressedList[i] );
  }
  // cout << endl;
  delete[] uncompressedList;
}

void tspRunner::getNeighborsFromMM(vector<uint> & dids, uint did){
  docNode d = this->compressedGraphNodes[did];
  if(d.length == 0){
    return;
  }
  uint * uncompressedList = new unsigned[d.length];
  decompressionVbytesInt(d.compressedList, uncompressedList, d.length);
  dids.push_back(uncompressedList[0]);
  // cout << uncompressedList[0]<<" ";
  for(uint i = 1; i < d.length; ++i){
    // cout << uncompressedList[i]<<" ";
    dids.push_back( dids.back() + uncompressedList[i] );
  }
  // cout << endl;
  delete[] uncompressedList;
}

void tspRunner::updateTermLists(uint newDid, uint did){
  vector<uint> terms;
  this->getDocFromMM(terms, did);

  for (uint i = 0; i < terms.size(); i++) {
    this->endDidVector[terms[i]] = newDid;
  }
  // cout << "test1" << endl;
}

//using binary search
bool tspRunner::getNextCandidate(uint newDid, uint did){
    float newSim = 0.0f;
    float samplingSim = 0.0f;
    float scaledSim = 0.0f;
    int numOfPairs = 0;
    vector<Edge> tmp;
    tmp.clear();

    vector<uint> neighbors;
    this->getNeighborsFromMM(neighbors, did);
    // cout << "t1" << endl;
    for(uint i = 0; i < neighbors.size(); i++){
      // cout << i << " " << neighbors[i] << endl;
      if(this->visited[neighbors[i]]){
        continue;
      }
      // cout << "t2" << endl;
      vector<uint> terms;
      this->getDocFromMM(terms, neighbors[i]);
      // cout << "t3" << endl;
      // cout << "test2" << endl;
      int docSize = terms.size();
      int size = min(100, docSize); //consider up to 100 terms in each document
      int step = max(1, docSize/100);
      // cout << d << " terms size: " << terms.size() << endl;
      for (uint j = 0; j < size; j+=step){
          uint endDid1 = this->endDidVector[terms[j]]; //find out which did term1 recently appeared in
          numOfPairs = this->proVector[terms[j]].size(); // find out how many pairs this term has in the LM
          int ln = (this->probPairsThreshold <= numOfPairs) ? this->probPairsThreshold : numOfPairs; //Consider up to 50 now
          for(int k = 0; k < ln; k++){ 
            probPair p = this->proVector[terms[j]][k]; //get term2 and the corresponding pair prob
            if(std::binary_search(terms.begin(), terms.end(), p.tid)){ // if term1 and term2 both appear in this document, no break point is created. do nothing
                continue;
            }
            uint endDid2 = this->endDidVector[p.tid]; //find out which did term2 recently appeared in
            if( endDid1 < endDid2) { //if term2 more recently appeared than term1, we have a break point here
                newSim += (-1)*p.exp;
            }
          }

          if(numOfPairs > this->probPairsThreshold){ // will do 50 sampling, if the size is between 50 and 100, then will just calculate them all
            if ( numOfPairs <= this->probPairsThresholdUp) {
              for(int k = this->probPairsThreshold; k < numOfPairs; k++){
               probPair p = this->proVector[terms[j]][k]; //get term2 and the corresponding pair prob
               if(std::binary_search(terms.begin(), terms.end(), p.tid)){ // if term1 and term2 both appear in this document, no break point is created. do nothing
                  continue;
               }
               uint endDid2 = this->endDidVector[p.tid]; //find out which did term2 recently appeared in
               if( endDid1 < endDid2) { //if term2 more recently appeared than term1, we have a break point here
                  newSim += (-1)*p.exp;
               }            
              }
            }else{// if the size is larger than 100, then will do the first 50 (for now), use the first 50 to estimate the remaining ones
              for(int k = this->probPairsThreshold; k < this->probPairsThresholdUp; k++){
                probPair p = this->proVector[terms[j]][k]; //get term2 and the corresponding pair prob
                if(std::binary_search(terms.begin(), terms.end(), p.tid)){ // if term1 and term2 both appear in this document, no break point is created. do nothing
                  continue;
                }
                uint endDid2 = this->endDidVector[p.tid]; //find out which did term2 recently appeared in
                if( endDid1 < endDid2) { //if term2 more recently appeared than term1, we have a break point here
                  samplingSim += (-1)*p.exp;
                } 
              }
              scaledSim =  samplingSim/(float)this->probPairsThreshold * (float)(numOfPairs - this->probPairsThresholdUp);  
              newSim += scaledSim;     
            }
          }
      }
      Edge e(neighbors[i], (float)newSim/docSize);
      tmp.push_back(e);
      newSim = 0.0f; //reset the similarity
    }
    // cout << "test3" << endl;
    /*get the did with the largest weight*/
    if(tmp.size()!=0){
      sort(tmp.begin(), tmp.end(), edgeComparator());
      // vector<uint> tmpNeighbors;
      // for(uint i = 0; i < tmp.size(); i++){
      //   tmpNeighbors.push_back(tmp[i]._docID);
      // }
      this->candidate = tmp[0]._docID;// the first did has the smallest punishment, this is what we wanted for next document
      return true;
    }else{
      this->candidate = 0;
      return false;
    }
}

void tspRunner::tsp(){
  // Vector to store the docid assignment (oldDocid -> newDocid).
  vector<uint> docidAssignment(this->numOfDocs + 1 , 0); //index 0 is useless

  // initialize visited vector
  this->visited.resize(this->numOfDocs + 1, false);
  this->visited[0] = true; //the first one is useless

  // initialize the endDid vector and the next candidate.
  this->endDidVector.resize(this->numOfTerms + 1, 0);
  this->candidate = 0;

  // Start with a partial path with a vertex/node from the largestSimilarityEdge.
  uint source = 1; // this is randomly assigned, start from zero now
  this->lastUnvisitedHasNeighbors = source + 1;
  this->lastUnvisitedHasNoNeighbors =  source + 1;

  docidAssignment[1] = source;
  this->visited[ source ] = true;
  this->numNodesSelected = 1; 


  while (this->numNodesSelected <= this->numOfDocs) {

    cout << "new did: " << this->numNodesSelected << " old did: " << source << endl;
    this->updateTermLists(this->numNodesSelected, source); //for all the terms in document source, update all the endDid information to the new did: numNodesSelected

    if (!this->getNextCandidate(this->numNodesSelected, source)) {//no available neighbor is selected for this document
      bool foundNextNodeHeuristic = false;
      for (uint i = this->lastUnvisitedHasNeighbors; i <= this->numOfDocs; i++) { 
        if ( (!visited[ i ]) && (compressedGraphNodes[i].length!=0) ) { // find next available node with neighbors
          source = i;
          this->candidate = i;
          this->lastUnvisitedHasNeighbors = min(this->candidate + 1, this->numOfDocs);
          foundNextNodeHeuristic = true;
          break;
        }
      }

      // In case that there are no nodes with edges left.
      if (!foundNextNodeHeuristic) {
        this->candidate = this->lastUnvisitedHasNoNeighbors;
        while (this->candidate < this->numOfDocs && visited[ this->candidate ]) {
          this->candidate++;
        }
        this->lastUnvisitedHasNoNeighbors = min(this->candidate + 1, this->numOfDocs);
      } 
    }

    // The next node has already been selected.
    this->visited[ this->candidate ] = true;
    this->numNodesSelected++;
    docidAssignment[ this->numNodesSelected ] = this->candidate;
    source = this->candidate;

    // if( this->numNodesSelected == 100000){
    //   break;
    // }
  }

  // for (int i = 1; i <= docidAssignment.size(); ++i) {
  //   cout << i << ": " << docidAssignment[i] << endl;
  // }
  FILE * lexDir = fopen(this->didMapping.c_str(), "w");
  if( lexDir == NULL){
    cout << "Problem! The file: " << this->didMapping << " could not be opened!" << endl;
    exit(0);
  }
  for(int i = 1; i < docidAssignment.size(); ++i){
    fprintf(lexDir, "%u %u\n", i, docidAssignment[i]);
  }
  fclose(lexDir);
  cout << "Writing out lsh reorder done \n";
}


void tspRunner::loadSortedPairsDA(){
  this->proVector.resize(this->numOfTerms+1);
  for(uint i = 0; i <= this->numOfTerms; i++){//termId start from 1, so use <=
    vector<probPair> t;
    t.clear();
    this->proVector.push_back(t);
  }

  FILE* fH = fopen(this->sortedPairs.c_str(), "r");
  if ( fH == NULL ) cout << this->sortedPairs << " file could not be opened" << endl;
  else cout << "Loading sorted pair probabilities from file " << this->sortedPairs << endl;
  uint tid1;
  uint tid2;
  float exponent = 0.0f;

  while( fscanf( fH,"%u %u %f\n", &tid1, &tid2, &exponent) != EOF ){
    probPair p(tid2, exponent);
    this->proVector[tid1].push_back(p);
  }
  fclose(fH);
}

void tspRunner::loadProbMassDA(){
  this->probMassVector.resize(this->numOfTerms+1, 0.0f);// initailize the vector
  FILE* fH = fopen(this->probMass.c_str(), "r");
  if ( fH == NULL ) cout << this->probMass << " file could not be opened" << endl;
  else cout << "Loading remaining probabilities mass larger than 200 from file " << this->probMass << endl;

  uint tid;
  float mass;

  while( fscanf( fH, "%u %f\n", &tid, &mass) != EOF){
    this->probMassVector[tid] = mass;
  }
  fclose(fH);
}

void tspRunner::loadSortedPairs(){

  FILE* fH = fopen(this->sortedPairs.c_str(), "r");
    if ( fH == NULL ) cout << this->sortedPairs << " file could not be opened" << endl;
    else cout << "Loading sorted pair probabilities from file " << this->sortedPairs << endl;

    uint tid1;
    uint tid2;
    double exponent = 0.0f;

    while( fscanf( fH,"%u%u%lf\n", &tid1, &tid2, &exponent) != EOF ) {
      probPair p(tid2, exponent);

      this->it = this->probMap.find(tid1);
      if (this->it != this->probMap.end()) {
        this->it->second.push_back(p);
      }else{
        vector<probPair> v;
        v.push_back(p);
        this->probMap[tid1] = v;
      }
    }

    fclose(fH);
    cout << this->probMap.size() << endl;
}

void tspRunner::loadProbMass(){

  FILE* fH = fopen(this->probMass.c_str(), "r");
  if ( fH == NULL ) cout << this->probMass << " file could not be opened" << endl;
  else cout << "Loading remaining probabilities mass larger than 200 from file " << this->probMass << endl;

  uint tid;
  double mass;

  while( fscanf( fH, "%u%lf\n", &tid, &mass) != EOF){
    this->probMassMap[tid] = mass;
  }
  fclose(fH);
  cout << this->probMassMap.size() << endl;
}
