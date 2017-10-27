#include "Merger.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstring>
#include <set>
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

int Merger::compressionVbytes(vector<uint> input){
   int compressedSize = 0;
   for (uint i = 0; i < input.size(); ++i){
          unsigned char byteArr[this->intSize];
          bool started = false;
          int x = 0;

          for (x = 0; x < this->intSize; x++) {
             byteArr[x] = (input[i]%128);
             input[i] /= 128;
          }

          for (x = this->intSize - 1; x > 0; x--) {
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

void Merger::generateDeltaGraph(){
  unsigned did;
  unsigned length;
  unsigned compressedSize;
  unsigned impact;
  unsigned long offset;
  unsigned long newOffset = 0;
  int breaker = 0;

  FILE * lexFile;
  lexFile = fopen(this->graphLex.c_str(), "r");
  if( lexFile == NULL) {
    cout << "Problem! The file: " << this->graphLex << " could not be opened!" << endl;
    exit(0);
  }

  FILE* IndexFile = fopen(this->graphIndex.c_str(),"r");
  if( IndexFile == NULL) {
    cout << "Problem! The file: " << this->graphIndex << " could not be opened!" << endl;
    exit(0);
  }

  while(fscanf(lexFile, "%u %u %u %u %lu\n", &did, &length, &compressedSize, &impact, &offset)!=EOF){
    cout << did << " " << length << " " << compressedSize << " " << impact << " " << offset << endl;
    unsigned char * compressedList = new unsigned char[compressedSize];
    unsigned * uncompressedList = new unsigned[length];
    fread(compressedList, 1, compressedSize, IndexFile);
    decompressionVbytesInt(compressedList, uncompressedList, length);
    vector<unsigned> neighbors;
    for(uint i = 0; i < length; ++i){
       // cout << uncompressedList[i]<<" ";
       neighbors.push_back(uncompressedList[i]);
    }
    // cout << "\n";
    sort(neighbors.begin(), neighbors.end());
    // for(uint i = 0; i < neighbors.size(); i++){
    // 	cout << neighbors[i] << " ";
    // }
    // cout << "\n";
    //todo
    // if(neighbors.size() > 50){
    // 	neighbors.resize(50);
    // 	length = 50;
    // }

    for(uint i = neighbors.size() - 1; i >=1; i--){
    	neighbors[i] = neighbors[i] - neighbors[i-1];
    }
    // for(uint i = 0; i < neighbors.size(); i++){
    // 	cout << neighbors[i] << " ";
    // }
    // cout << "\n";

    uint compressedSizeDelta = compressionVbytes(neighbors);


    didsGraph.push_back(did);
    lengthsGraph.push_back(length);
    compressedSizesGraph.push_back(compressedSizeDelta);
    offsetsGraph.push_back(newOffset);
    newOffset += compressedSizeDelta;

    // breaker++;
    // if(breaker == 10){
    //   break;
    // }
  }
  cout << "Graph entries loaded: " << breaker << endl;

  fclose(lexFile);
  fclose(IndexFile);

  writeDeltaIndex();
}

void Merger::verifyGraph(string lex_path, string index_path){
  unsigned did;
  unsigned length;
  unsigned compressedSize;
  unsigned impact;
  unsigned long offset;
  int breaker = 0;

  FILE * lexFile;
  lexFile = fopen(lex_path.c_str(), "r");
  if( lexFile == NULL) {
    cout << "Problem! The file: " << lex_path << " could not be opened!" << endl;
    exit(0);
  }

  FILE* IndexFile = fopen(index_path.c_str(),"r");
  if( IndexFile == NULL) {
    cout << "Problem! The file: " << this->graphIndex << " could not be opened!" << endl;
    exit(0);
  }

  while(fscanf(lexFile, "%u %u %u %u %lu\n", &did, &length, &compressedSize, &impact, &offset)!=EOF){
    // cout << did << " " << length << " " << compressedSize << " " << impact << " " << offset << endl;
    unsigned char * compressedList = new unsigned char[compressedSize];
    unsigned * uncompressedList = new unsigned[length];
    fread(compressedList, 1, compressedSize, IndexFile);
    decompressionVbytesInt(compressedList, uncompressedList, length);
    vector<unsigned> neighbors;
    for(uint i = 0; i < length; ++i){
       // cout << uncompressedList[i]<<" ";
       neighbors.push_back(uncompressedList[i]);
    }
    // cout << "\n";
    sort(neighbors.begin(), neighbors.end());
    //todo
    if(neighbors.size() > 50){
      neighbors.resize(50);
      length = 50;
    }
    cout << did << " " << length << " " << compressedSize << " " << impact << " " << offset << endl;
    for(uint i = 0; i < neighbors.size(); i++){
      cout << neighbors[i] << " ";
    }
    cout << "\n";

    // for(uint i = neighbors.size() - 1; i >=1; i--){
    //   neighbors[i] = neighbors[i] - neighbors[i-1];
    // }
    // for(uint i = 0; i < neighbors.size(); i++){
    //   cout << neighbors[i] << " ";
    // }
    // cout << "\n";
    delete[] compressedList;
    delete[] uncompressedList;
    breaker++;
    if(breaker == 10){
      break;
    }
  }
  cout << "Graph entries loaded: " << breaker << endl;

  fclose(lexFile);
  fclose(IndexFile);
}


void Merger::verifyDeltaGraph (string lex_path, string index_path) {
  cout << "Loading Delta Compressded Graphs\n";
  uint did;
  uint length;
  uint compressedSize;
  unsigned long offset;

  FILE * lexFile;
  lexFile = fopen(lex_path.c_str(), "r");
  if( lexFile == NULL){
    cout << "Problem! The file: " << lex_path << " could not be opened!" << endl;
    exit(0);
  }

  FILE* IndexFile = fopen(index_path.c_str(),"r");
  if( IndexFile == NULL) {
    cout << "Problem! The file: " << index_path << " could not be opened!" << endl;
    exit(0);
  }

  // for(uint i = 0; i <= this->numOfDocs; i++){
  //   unsigned char * p = NULL;
  //   docNode t(p, 0);
  //   this->compressedGraphNodes.push_back(t);
  // }

  while(fscanf(lexFile, "%u %u %u %lu\n", &did, &length, &compressedSize, &offset)!=EOF){
    // cout << did << " " << length << " " << compressedSize << " " << offset << endl;
    unsigned char * cp = new unsigned char[compressedSize];
    fread(cp, 1, compressedSize, IndexFile);

    uint * uncompressedList = new unsigned[length];
    decompressionVbytesInt(cp, uncompressedList, length);
    vector<uint> dids;
    dids.push_back(uncompressedList[0]);
    // cout << uncompressedList[0]<<" ";
    for(uint i = 1; i < length; ++i){
      // cout << uncompressedList[i]<<" ";
      dids.push_back( dids.back() + uncompressedList[i] );
    }
    // cout << endl;
    // cout << dids.size() << endl;
    for(uint i = 0; i < dids.size(); i++){
      cout << dids[i] << " ";
    }
    cout << endl;
    delete[] cp;
    delete[] uncompressedList;
    // if(did == 10){
    //   break;
    // }
  }

  cout << index_path << " Verification Done\n";
  fclose(lexFile);
  fclose(IndexFile);

}

void Merger::writeDeltaIndex(){
  /*dump buffer to file*/
  dumpToFile(this->deltaGraphIndex, this->compressedList.begin(), this->compressedList.end());
  /*dump lexicon to file*/
  FILE * lexDir = fopen(this->deltaGraphLex.c_str(), "w");
  if( lexDir == NULL){
     cout << "Problem! The file: " << this->deltaGraphLex << " could not be opened!" << endl;
     exit(0);
  }
  for(uint i = 0; i < didsGraph.size(); ++i){
     fprintf(lexDir, "%u %u %u %lu\n", didsGraph[i], lengthsGraph[i], compressedSizesGraph[i], offsetsGraph[i]);
  }
  fclose(lexDir);
    cout << "delta graph generated" << endl;
}

void Merger::writeMergedIndex(){
	/*dump buffer to file*/
	dumpToFile(this->mergedGraphIndex, this->compressedList.begin(), this->compressedList.end());
	/*dump lexicon to file*/
	FILE * lexDir = fopen(this->mergedGraphLex.c_str(), "w");
	if( lexDir == NULL){
	   cout << "Problem! The file: " << this->mergedGraphLex << " could not be opened!" << endl;
	   exit(0);
	}
	for(uint i = 0; i < didsGraph.size(); ++i){
	   fprintf(lexDir, "%u %u %u %lu\n", didsGraph[i], lengthsGraph[i], compressedSizesGraph[i], offsetsGraph[i]);
	}
	fclose(lexDir);
  	cout << "delta graph generated" << endl;
}


void Merger::loadDeltaGraph(){
  cout << "Loading Delta Compressded Graphs\n";
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

  // for(uint i = 0; i <= this->numOfDocs; i++){
  //   unsigned char * p = NULL;
  //   docNode t(p, 0);
  //   this->compressedGraphNodes.push_back(t);
  // }
  this->lshNeighborArray.resize(this->numDocs+1);

  while(fscanf(lexFile, "%u %u %u %lu\n", &did, &length, &compressedSize, &offset)!=EOF){
    // cout << did << " " << length << " " << compressedSize << " " << offset << endl;
    unsigned char * cp = new unsigned char[compressedSize];
    fread(cp, 1, compressedSize, IndexFile);

    uint * uncompressedList = new unsigned[length];
  	decompressionVbytesInt(cp, uncompressedList, length);
  	vector<uint> dids;
  	dids.push_back(uncompressedList[0]);
  	// cout << uncompressedList[0]<<" ";
  	for(uint i = 1; i < length; ++i){
    	// cout << uncompressedList[i]<<" ";
    	dids.push_back( dids.back() + uncompressedList[i] );
  	}
  	// cout << endl;
  	// cout << dids.size() << endl;
    // for(int i = 0; i < dids.size(); i++){
    //   cout << dids[i] << " ";
    // }
    this->lshNeighborArray[did] = dids;
    // cout << endl;
    delete[] cp;
  	delete[] uncompressedList;
    // if(did == 10){
    //   break;
    // }
  }

  cout << deltaGraphIndex << " Loading Done\n";
  fclose(lexFile);
  fclose(IndexFile);

}

void Merger::loadUrls(){
    // Lexicon.
    this->docLexFP = fopen(this->lexFileTSPQi.c_str(), "r");
    if (this->docLexFP == NULL) {
       cout << "Could not open file: " << this->lexFileTSPQi << ". Exiting." << endl;
       exit(0);
    }

    // assert(numOfDocsToLoad > 0 && numOfDocsToLoad <= CONST::numDocs);
    vector<docLexTSP> dlex;
    
    // Temporary vars for fscanf.
    vector<unsigned long> tmpVec(this->docIndexLexElems, 0);
    char baseUrl[ 10240 ];
    char fullOrSuffixUrl[ 10240 ];
    char docidAr[ 1024 ];
    unsigned long docLen = 0;
    unsigned int docsLoaded = 0;
    // Document doc;
    
    while (fscanf( this->docLexFP, "%s%lu%lu%lu%lu%s%s\n",
                  docidAr, &tmpVec[0], &tmpVec[1], &tmpVec[2], &docLen, baseUrl, fullOrSuffixUrl) != EOF) {
        string docid(docidAr);
        // cmpList tmpList(docid, StrToUint(docid), (unsigned int)tmpVec[0], 0, tmpVec[1], tmpVec[2]);
        // struct docLexTSP dlexentry;
        // dlexentry.cmList = tmpList;
        // dlexentry.docId = StrToUint(docid);
        // dlexentry.docLen = docLen;
        // dlexentry.url = fullOrSuffixUrl;
        // dlexentry.baseUrl = baseUrl;
        // this->dLex.push_back(dlexentry);
        ++docsLoaded;
        struct urlDocId sdoctmp;
        sdoctmp.url = fullOrSuffixUrl;
        // sdoctmp.docId = StrToUint(docid);
        sdoctmp.docId = docsLoaded;
        this->urls.push_back(sdoctmp);  //and the URL of url-docid map

        // if (docsLoaded == 1000) break; // Termination condition.
    }
    fclose(this->docLexFP);
    // Reporting.
    cout << "# lexicon entries loaded: " << docsLoaded << endl;
}

void Merger::getUrlNeighbors(){
  sort(this->urls.begin(), this->urls.end(), comparator());
  uint topRange = 0;
  uint botRange = 0;
  uint boundary = this->urls.size();
  // uint boundary = 500;
  this->urlNeighborArray.resize(this->numDocs+1);
  // for(int i = 0; i < this->numDocs+1; i++){
  //   vector<unsigned int> tmp;
  //   this->urlNeighborArray.push_back(tmp);
  // }
  for (uint i = 0; i < boundary; i++) {
    uint did = this->urls[i].docId;
    vector<unsigned int> dids;

    if(i+1 > this->urlRange){
      if( boundary - i - 1 <= this->urlRange){
        botRange = boundary - i - 1;
        topRange = this->urlRange + this->urlRange - (boundary - i - 1);
      }else{
        botRange = this->urlRange;
        topRange = this->urlRange;
      }
    }else{
      topRange = i;
      botRange = this->urlRange + this->urlRange - i;
    }
    // cout << i <<" " << topRange << " " << botRange << " ";
    for(uint j = 0; j < topRange; j++){
      dids.push_back(this->urls[i-j-1].docId);
    }
    for(uint j = 0; j < botRange; j++){
      dids.push_back(this->urls[i+j+1].docId);
    }
    // cout << dids.size() << endl;
    this->urlNeighborArray[did] = dids;
  }
}


void Merger::WriteIndex(string lex_path, string index_path){
  /*dump buffer to file*/
  dumpToFile(index_path, this->compressedList.begin(), this->compressedList.end());
  /*dump lexicon to file*/
  FILE * lexDir = fopen(lex_path.c_str(), "w");
  if( lexDir == NULL){
     cout << "Problem! The file: " << lex_path << " could not be opened!" << endl;
     exit(0);
  }
  for(uint i = 0; i < didsGraph.size(); ++i){
     fprintf(lexDir, "%u %u %u %lu\n", didsGraph[i], lengthsGraph[i], compressedSizesGraph[i], offsetsGraph[i]);
  }
  fclose(lexDir);
    cout << "url delta graph generated" << endl;
}

void Merger::CompressUrlGraph(){
  unsigned long newOffset = 0;
  for(uint i = 1; i <= this->numDocs; i++){
  // for(int i = 1; i <= 10; i++){
    vector<uint> dids;
    dids = this->urlNeighborArray[i];

    sort(dids.begin(), dids.end());

    // for(uint n = 0; n < dids.size(); n++){
    //   cout << dids[n] << " ";
    // }
    // cout << endl;

    for(uint n = dids.size() - 1; n >=1; n--){
      dids[n] = dids[n] - dids[n-1];
    }
    // for(uint n = 0; n < dids.size(); n++){
    //   cout << dids[n] << " ";
    // }
    // cout << endl;
    
    uint compressedSizeDelta = compressionVbytes(dids);
    cout << i << " " << dids.size() << " " << compressedSizeDelta << " " << newOffset << endl;
    didsGraph.push_back(i);
    lengthsGraph.push_back(dids.size());
    compressedSizesGraph.push_back(compressedSizeDelta);
    offsetsGraph.push_back(newOffset);
    newOffset += compressedSizeDelta;
  }
  this->WriteIndex(kUrlLex, kUrlIndex);
}

void Merger::mergeNeighbors(){
  unsigned long newOffset = 0;
  // for(int i = 1; i <= this->numDocs; i++){
    for(int i = 1; i <= 10; i++){
    vector<uint> dids;
    uint j = 0;
    for(j = 0; j < this->lshNeighborArray[i].size(); j++){
      dids.push_back(this->lshNeighborArray[i][j]);
    }

    if( j < 100 ){
      for(uint k = 0; k < this->urlNeighborArray[i].size(); k++){
        dids.push_back(this->urlNeighborArray[i][k]);
        //todo reach 100 break
        if(j + k >= 99){
          break;
        }
      }
    }

    sort(dids.begin(), dids.end());

    for(uint n = dids.size() - 1; n >=1; n--){
      dids[n] = dids[n] - dids[n-1];
    }
    for(uint n = 0; n < dids.size(); n++){
      cout << dids[n] << " ";
    }
    cout << endl;
    uint compressedSizeDelta = compressionVbytes(dids);
    cout << i << " " << dids.size() << " " << compressedSizeDelta << " " << newOffset << endl;
    didsGraph.push_back(i);
    lengthsGraph.push_back(dids.size());
    compressedSizesGraph.push_back(compressedSizeDelta);
    offsetsGraph.push_back(newOffset);
    newOffset += compressedSizeDelta;
  }
  // writeMergedIndex();
}


void Merger::mergeGraph(){
    this->loadUrls();
    this->getUrlNeighbors();
    this->loadDeltaGraph();
    this->mergeNeighbors();
}

void Merger::orderReMap(){
  this->loadRandomOrder();
  this->loadOriginalOrder();
  this->getNewOrder();
  // int k = 0;
  // for ( auto it = this->originalOrderMap.begin(); it != this->originalOrderMap.end(); ++it ){
  //   cout << it->first << ":" << it->second << endl;
  //   k++;
  //   if(k == 10){
  //     break;
  //   }
  // }

  string dir = this->newOrder;
  FILE * lexDir = fopen(dir.c_str(), "w");
  if( lexDir == NULL){
    cout << "Problem! The file: " << dir << " could not be opened!" << endl;
    exit(0);
  }
  for ( auto it = this->originalOrderMap.begin(); it != this->originalOrderMap.end(); ++it ){
    fprintf(lexDir, "%u %u\n", it->first, it->second);
  }
  fclose(lexDir);
}

void Merger::loadRandomOrder(){
  FILE * lexFile;
  // lexFile = fopen(this->urlOrdering.c_str(), "r"); //url
  // lexFile = fopen(this->randomOrdering.c_str(), "r"); //random
  // lexFile = fopen(this->tspOrdering.c_str(), "r"); //tsp
  lexFile = fopen(this->randomOrder.c_str(), "r"); //test
  if( lexFile == NULL) {
    cout << "Problem! The file: " << this->randomOrder << " could not be opened!" << endl;
    exit(0);
  }
  uint did = 0;
  uint newDid = 0;
  while(fscanf(lexFile, "%u %u", &did, &newDid)!=EOF){
    // cout << did << " " << newDid << endl;
    this->randomOrderMap.insert(pair<unsigned int, unsigned int>(did, newDid));
  }
  cout <<"did mapping map size: " << this->randomOrderMap.size() <<endl;
  fclose(lexFile);
}

void Merger::loadOriginalOrder(){
  FILE * lexFile;
  // lexFile = fopen(this->urlOrdering.c_str(), "r"); //url
  // lexFile = fopen(this->randomOrdering.c_str(), "r"); //random
  // lexFile = fopen(this->tspOrdering.c_str(), "r"); //tsp
  lexFile = fopen(this->tspOrder.c_str(), "r"); //test
  if( lexFile == NULL) {
    cout << "Problem! The file: " << this->tspOrder << " could not be opened!" << endl;
    exit(0);
  }
  uint did = 0;
  uint newDid = 0;
  while(fscanf(lexFile, "%u %u", &did, &newDid)!=EOF){
    // cout << did << " " << newDid << endl;
    this->originalOrderMap.insert(pair<unsigned int, unsigned int>(did, newDid));
  }
  cout <<"did mapping map size: " << this->originalOrderMap.size() <<endl;
  fclose(lexFile);
}

void Merger::getNewOrder(){
  for ( auto it = this->originalOrderMap.begin(); it != originalOrderMap.end(); ++it ){
      map<unsigned int, unsigned int>::iterator it2;
      it2 = this->randomOrderMap.find(it->second);
      uint newDid = 0;
      if(it2!=this->randomOrderMap.end()){
        newDid = it2->second;
      }else{
        newDid = it->second;
      }
      it->second = newDid;
  }
}

void Merger::loadDocLength(){
    this->doclen = new unsigned int[this->MAXD];
    for(int i = 0; i < this->MAXD; i++){
      doclen[i] = 0;
    }
    FILE * lenFile;
    lenFile = fopen(this->docLenFileDir.c_str(), "r");
    if( lenFile == NULL) {
      cout << "Problem! The file: " << this->docLenFileDir << " could not be opened!" << endl;
      exit(0);
    }
    uint did = 0;
    uint length = 0;
    uint newDid = 0;

    while(fscanf(lenFile, "%u %u\n", &did, &length)!=EOF){
      unordered_map<unsigned int, unsigned int>::iterator it;
      it = this->newOrderMap.find( did );
      if(it == this->newOrderMap.end()){
        // newDid = did;
        continue;
      }else{
        newDid = this->newOrderMap[ did ];
      }
      // this->docLengthsMap.insert(pair<unsigned int, unsigned int>(newDid, length));
      // cout << did << " " << newDid << " " << length << endl;
      doclen[newDid] = length;
    }
    fclose(lenFile);
}

void Merger::loadNewOrdering(){
  FILE * lexFile;
  lexFile = fopen(this->ordering.c_str(), "r");
  if( lexFile == NULL) {
    cout << "Problem! The file: " << this->ordering << " could not be opened!" << endl;
    exit(0);
  }
  uint did = 0;
  uint newDid = 0;
  while(fscanf(lexFile, "%u %u", &newDid, &did)!=EOF){
    // cout << did << " " << newDid << endl;
    this->newOrderMap.insert(pair<unsigned int, unsigned int>(did, newDid - 1));
  }
  // cout <<"did mapping map size: " << this->newOrderMap.size() <<endl;
  fclose(lexFile);
}

void Merger::writeOutDocLengthFile(){ //save the cout as file
   for(int i = 0; i < this->MAXD; i++){
     cout << i << " " << doclen[i] << endl;
   }
}

void Merger::docLengthRemap(){
   this->loadNewOrdering();
   this->loadDocLength();
   this->writeOutDocLengthFile();
   //todo output
}

void Merger::BuildURLOnlyGraph(){
   this->loadUrls();
   this->getUrlNeighbors();
   this->CompressUrlGraph();
}

/***************************************************************************
LMMaker function implementation
***************************************************************************/

void LMMaker::buildTermIDQueryLog(){ //the cout directly generate the output
  this->LoadLex();
  FILE * lexFile;
  lexFile = fopen(kRawQuery.c_str(), "r");
  if(lexFile == NULL){
    cout << "Problem! The file: " << kRawQuery << " could not be opened!" << endl; 
  }
  char terms[ 1024 ];
  while(fgets(terms, sizeof terms, lexFile)!= NULL){
    string tmp(terms);
    // printf("%s\n", term);
    // cout << tmp;
    stringstream ss(tmp);
    string buf;
    vector<string> termIds;
    while (ss >> buf){
        termIds.push_back(buf);
    }
    for(uint i = 0; i < termIds.size(); i++){
      unordered_map<string, LexInfo>::iterator it;
      it = this->lexMap.find(termIds[i]);
      if(it!=this->lexMap.end()){
        cout << it->second.termId << " ";
      }
    }
    cout << endl;
  }
  fclose(lexFile);
}

// void LMMaker::queryTermIDSelector(){ //use the cout as output
//   this->LoadLex();
//   FILE * lexFile;
//   lexFile = fopen(kQueryPool.c_str(), "r");
//   if(lexFile == NULL){
//     cout << "Problem! The file: " << kQueryPool << " could not be opened!" << endl; 
//   }
//   uint k = 0;
//   char terms[ 1024 ];
//   while(fgets(terms, sizeof terms, lexFile)!= NULL){
//     if(k%3 == 0){
//       string tmp(terms);
//       // printf("%s\n", term);
//       // cout << tmp;
//       stringstream ss(tmp);
//       string buf;
//       vector<string> termIds;
//       while (ss >> buf){
//           termIds.push_back(buf);
//       }
//       for(uint i = 0; i < termIds.size(); i++){
//         unordered_map<string, LexInfo>::iterator it;
//         it = this->lexMap.find(termIds[i]);
//         if(it!=this->lexMap.end()){
//           cout << it->second.termId << " ";
//         }
//       }
//       cout << endl;
//     }
//     // if(k == 2999){
//     if(k == 29){
//       break;
//     }
//     k++;
//   }
//   fclose(lexFile);
// }

// void LMMaker::rawQuerySelector(){ //use the cout as output
//   FILE * lexFile;
//   lexFile = fopen(kQueryPool.c_str(), "r");
//   if(lexFile == NULL){
//     cout << "Problem! The file: " << kQueryPool << " could not be opened!" << endl; 
//   }
//   uint k = 0;
//   char terms[ 1024 ];
//   // string exception = "group home in new york";
//   while(fgets(terms, sizeof terms, lexFile)!= NULL){
//     if(k%3 == 0){
//       string tmp(terms);
//       // printf("%s\n", term);
//       cout << tmp;
//     }
//     // if(k == 3000){
//     if(k == 29){
//       break;
//     }
//     k++;
//   }
//   fclose(lexFile);
// }

void LMMaker::buildSmallLex(const string input_query, const string output_lex){//the cout directly generate the output
  this->LoadLex();
  FILE * queryFile = fopen(input_query.c_str(), "r");
  if(queryFile == NULL){
    cout << "Problem! The file: " << input_query << " could not be opened!" << endl; 
  }

  FILE * lexFile = fopen(output_lex.c_str(), "w");
  if( lexFile == NULL){
     cout << "Problem! The file: " << output_lex << " could not be opened!" << endl;
     exit(0);
  }

  char term[ 1024 ];
  while(fscanf(queryFile, "%s", term)!=EOF){
    string tmp(term);
    // printf("%s\n", term);
    // cout << tmp << " ";
    fprintf(lexFile, "%s ", term);
    unordered_map<string, LexInfo>::iterator it;
    it = this->lexMap.find(tmp);
    if(it!=this->lexMap.end()){
      // cout << it->second.termId << " " << it->second.listLen << " " << it->second.offset << " " 
      // << it->second.listLenInBytes << endl;
      fprintf(lexFile, "%u %u %lu %u\n", it->second.termId, it->second.listLen, it->second.offset, it->second.listLenInBytes);
    }
  }
  fclose(queryFile);
  fclose(lexFile);
}

void LMMaker::LoadLex(){
  FILE * lexFile;
  lexFile = fopen(kWholeLex.c_str(), "r");
  if(lexFile == NULL){
    cout << "Problem! The file: " << kWholeLex << " could not be opened!" << endl;
  }

  unsigned long long offset = 0;
  char term[300];
  uint termId = 0;
  uint listLen = 0;
  uint listLenInBytes = 0;
  uint tmp1 = 0;
  uint tmp2 = 0;
  uint tmp3 = 0;
  uint tmp4 = 0;
  uint tmp5 = 0;

  //term termID listLength totalOccurrences offset listLenBytes posOffset posListLen contOffset contListLen 
  while(fscanf(lexFile, "%s %u %u %u %llu %u %u %u %u %u\n", term, &termId, &listLen, &tmp1, &offset, &listLenInBytes, &tmp2, &tmp3, &tmp4, &tmp5)!=EOF){
    string termName(term);
    // cout << termName <<" "<< termId << " " << listLen << " " << listLenInBytes << " " << offset << endl;
    LexInfo l(termId, listLen, listLenInBytes, offset);
    this->lexMap.insert(pair<string, LexInfo>(termName, l));
  }

  fclose(lexFile);
}

void LMMaker::LoadSmallLex(string input_lex){
  FILE* lexFile;
  lexFile = fopen(input_lex.c_str(), "r");
  if(lexFile == NULL){
    cout << "Problem! The file: " << input_lex << " could not be opened!" << endl;
  }
  char term[1001];
  uint termID;
  uint listLen;
  uint listLenInBytes;
  unsigned long long offset = 0;
  while(fscanf(lexFile, "%s %u %u %llu %u\n", term, &termID, &listLen, &offset, &listLenInBytes)!=EOF) {
    string termName(term);
    LexInfo l(termID, listLen, listLenInBytes, offset);
    this->lexMap.insert(pair<string, LexInfo>(termName, l));
  }
}

bool SortBySecond(const pair<string, int> p1, const pair<string, int>p2){
  return (p1.second < p2.second);
}

void LMMaker::GetTwoTermQueries(const std::string lex_file, const std::string input_query,
  const std::string common_term_file, const std::string output_query){
  LoadSmallLex(lex_file);

  /*undesired terms*/
  std::set<std::string> common_terms;
  ifstream ctFile(common_term_file);
  if(ctFile) {
    char term[500];
    while(ctFile.getline(term, 500)) {
      string t(term);
      common_terms.insert(t);
      std::cout << t << std::endl;
    }
  }
  ctFile.close();
  // exit(0);

  ifstream inputQueryFile(input_query);
  ofstream queryFile(output_query);

  if(inputQueryFile){
    char query[1001];
    // string query;
    while(inputQueryFile.getline(query, 1001)){
      char* terms;
      terms = strtok(query, " ");
      vector<pair<string, uint>> vec;
      while(terms){
        string termName(terms);
        // std::cout << termName << std::endl;
        auto it1 = common_terms.find(termName);
        if (it1 != common_terms.end()) {
          terms = strtok(NULL, " ");
          continue;
        }
        auto it = this->lexMap.find(termName);
        if(it != this->lexMap.end()) {
          // cout << it->second.listLen << " ";
          /*remove undesired terms, the, and*/
          vec.push_back(pair<string, uint>(termName, it->second.listLen));
        }
        terms = strtok(NULL, " ");
      }
      // cout << endl;
      sort(vec.begin(), vec.end(), SortBySecond);
      if(vec.size() > 2) {
        vec.resize(2);
      }
      if(vec.size() < 2) {
        continue;
      }
      if(vec[0] == vec[1]) {
        continue;
      }
      for(uint i = 0; i < vec.size(); i++){
        queryFile << vec[i].first << " ";
      }
      queryFile << "\n";
    }
  } 
  inputQueryFile.close();
  queryFile.close();
}

void LMMaker::RandomGetOneKQuery(const std::string input_query,
  const std::string output_query) {
  ifstream in_file(input_query);
  std::vector<string> queries;
  char query[1001];
  if(in_file) {
    while(in_file.getline(query, 1001)) {
      string terms(query);
      queries.push_back(terms);
      // std::cout << terms << std::endl;
    }
  }
  in_file.close();
  // std::cout << queries.size() << std::endl;

  ofstream out_file(output_query);
  std::set<string> random_quries; 
  while(random_quries.size() < 1000) {
    string r = *select_randomly(queries.begin(), queries.end());
    auto it = random_quries.find(r);
    if(it==random_quries.end()){
      random_quries.insert(r);
      out_file << r << std::endl;
    }
  }
  std::cout << random_quries.size() << std::endl;
}

void LMMaker::GenerateIdealLM(const std::string query_trace,
  const std::string lm_path){
  std::cout << "making ideal lm\n";
  //todo, for now just use the pairs, treat each of them with weight 1
  std::map<std::string, int> lm_map;

  ifstream queryFile(query_trace);
  if(queryFile) {
    char query[1000];
    while(queryFile.getline(query, 1000)) {
      vector<string> terms;
      char* term;
      term = strtok(query, " ");
      while(term) {
        // std::cout << term << std::endl;
        terms.push_back(term);
        term = strtok(NULL, " ");
      }
      // sort(terms.begin(), terms.end());
      auto it = lm_map.find(terms[0]+ " " +terms[1]);
      if(it == lm_map.end()) {
        lm_map[terms[0]+ " " +terms[1]] = 1;
      }else{
        lm_map[terms[0]+ " " +terms[1]]++;
      }
      auto it1 = lm_map.find(terms[1]+ " " +terms[0]);
      if(it1 == lm_map.end()) {
        lm_map[terms[1]+ " " +terms[0]] = 1;
      }else {
        lm_map[terms[1]+ " " +terms[0]]++;
      }
    }
  }
  queryFile.close();

  ofstream lmFile(lm_path);
  for(auto it=lm_map.begin(); it!=lm_map.end(); it++) {
    lmFile << it->first << " " << it->second << endl;
  }
  lmFile.close();
}

void LMMaker::TransformLM(const string lex_path,
  const string lm_path, const string lm_path_termID){
  std::cout << "transform lm into TermIDs\n";
  map<string, int> term_table;
  FILE* lexFile = fopen(kTermTable.c_str(), "r");
  if(!lexFile) {
    std::cout << kTermTable << " can not be opened\n";
  }
  char term[1000];
  uint tid;
  while(fscanf(lexFile, "%s %u", term, &tid)!=EOF) {
    term_table[term] = tid;
  }
  fclose(lexFile);
  cout << "term table loading done\n";

  ifstream lmFile(lm_path);
  ofstream lmTermIDFile(lm_path_termID);
  if(lmFile && lmTermIDFile) {
    char lm[1000];
    while(lmFile.getline(lm, 1000)) {
      char* term;
      term = strtok(lm, " ");
      int num = 0; //the third one is freq we don't map it
      while(term) {
        // cout << term << " ";
        if(2 == num++) {
          lmTermIDFile << term << endl;
          break;
        }else{
          auto it = term_table.find(term);
          if(it!=term_table.end()) {
            // cout << it->second << " ";
            lmTermIDFile << it->second << " ";
          }
          term = strtok(NULL, " ");
        }
      }
    }
  }
  lmFile.close();
  lmTermIDFile.close();
}

/*utils for gov2 dataset*/
void Gov2Utils::LoadLex(){
  cout << "loading word table for GOV2 index\n";
  FILE* lexFile;
  lexFile = fopen(kGov2WordTable.c_str(), "r");
  if(lexFile == NULL) {
    cout << "Can not open file: " << kGov2WordTable << endl;
  }
  char term[300];
  uint termID;
  while(fscanf(lexFile, "%s %u", term, &termID)!=EOF) {
    string termName(term);
    // cout << term << " " << termID << endl;
    // this->lexMap.insert(pair<string, LexInfo>(termName, l));
    lex_map.insert(pair<string, int>(termName, termID));
  }
  fclose(lexFile);
  cout << "loading word table for GOV2 done\n";
}

void Gov2Utils::Gov2SmallLex(const string input_query, const string output_lex){
  LoadLex();
  cout << "start building small lex for GOV2" << endl;
  FILE * queryFile = fopen(input_query.c_str(), "r");
  if(queryFile == NULL){
    cout << "Problem! The file: " << input_query << " could not be opened!" << endl; 
  }

  FILE * lexFile = fopen(output_lex.c_str(), "w");
  if( lexFile == NULL){
     cout << "Problem! The file: " << output_lex << " could not be opened!" << endl;
     exit(0);
  }

  char term[ 1024 ];
  while(fscanf(queryFile, "%s", term)!=EOF){
    string tmp(term);
    // printf("%s\n", term);
    // cout << tmp << " ";
    unordered_map<string, int>::iterator it;
    it = lex_map.find(tmp);
    if(it!=lex_map.end()) {
      // cout << it->second.termId << " " << it->second.listLen << " " << it->second.offset << " " 
      // << it->second.listLenInBytes << endl;
      fprintf(lexFile, "%s %u\n", term, it->second);
    }
  }
  fclose(queryFile);
  fclose(lexFile);
  cout << "GOV2 small lex building done" << endl;
}