#include "Merger.h"
#include <iostream>
#include <algorithm>
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
   for (int i = 0; i < input.size(); ++i){
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

void Merger::verifyGraph(){
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

    breaker++;
    if(breaker == 10){
      break;
    }
  }
  cout << "Graph entries loaded: " << breaker << endl;

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
  for(int i = 0; i < didsGraph.size(); ++i){
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
	for(int i = 0; i < didsGraph.size(); ++i){
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

void Merger::mergeNeighbors(){
  unsigned long newOffset = 0;
  // for(int i = 1; i <= this->numDocs; i++){
    for(int i = 1; i <= 10; i++){
    vector<uint> dids;
    int j = 0;
    for(j = 0; j < this->lshNeighborArray[i].size(); j++){
      dids.push_back(this->lshNeighborArray[i][j]);
    }

    if( j < 100 ){
      for(int k = 0; k < this->urlNeighborArray[i].size(); k++){
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

void Merger::buildTermIDQueryLog(){ //the cout directly generate the output
  this->loadLex();
  FILE * lexFile;
  lexFile = fopen(this->rawQuery.c_str(), "r");
  if(lexFile == NULL){
    cout << "Problem! The file: " << this->rawQuery << " could not be opened!" << endl; 
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

void Merger::buildSmallLex(){//the cout directly generate the output
  this->loadLex();
  FILE * lexFile;
  lexFile = fopen(this->rawQuery.c_str(), "r");
  if(lexFile == NULL){
    cout << "Problem! The file: " << this->rawQuery << " could not be opened!" << endl; 
  }
  char term[ 1024 ];
  while(fscanf(lexFile, "%s", term)!=EOF){
    string tmp(term);
    // printf("%s\n", term);
    cout << tmp << " ";
    unordered_map<string, LexInfo>::iterator it;
    it = this->lexMap.find(tmp);
    if(it!=this->lexMap.end()){
      cout << it->second.termId << " " << it->second.listLen << " " << it->second.offset << " " << it->second.listLenInBytes << endl;
    }
  }
  fclose(lexFile);
}


void Merger::loadLex(){
  FILE * lexFile;
  lexFile = fopen(this->wholeLex.c_str(), "r");
  if(lexFile == NULL){
    cout << "Problem! The file: " << this->wholeLex << " could not be opened!" << endl;
  }

  unsigned long offset = 0;
  char * term;
  term = new char[300];
  uint termId = 0;
  uint listLen = 0;
  uint listLenInBytes = 0;
  uint tmp1 = 0;
  uint tmp2 = 0;
  uint tmp3 = 0;
  uint tmp4 = 0;
  uint tmp5 = 0;

  //term termID listLength totalOccurrences offset listLenBytes posOffset posListLen contOffset contListLen 
  while(fscanf(lexFile, "%s %u %u %u %lu %u %u %u %u %u\n", term, &termId, &listLen, &tmp1, &offset, &listLenInBytes, &tmp2, &tmp3, &tmp4, &tmp5)!=EOF){
    string termName(term);
    // cout << termName <<" "<< termId << " " << listLen << " " << listLenInBytes << " " << offset << endl;
    LexInfo l(termId, listLen, listLenInBytes, offset);
    this->lexMap.insert(pair<string, LexInfo>(termName, l));
  }

  fclose(lexFile);
}

void Merger::queryTermIDSelector(){ //use the cout as output
  this->loadLex();
  FILE * lexFile;
  lexFile = fopen(this->queryPool.c_str(), "r");
  if(lexFile == NULL){
    cout << "Problem! The file: " << this->queryPool << " could not be opened!" << endl; 
  }
  uint k = 0;
  char terms[ 1024 ];
  while(fgets(terms, sizeof terms, lexFile)!= NULL){
    if(k%3 == 0){
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
    // if(k == 2999){
    if(k == 29){
      break;
    }
    k++;
  }
  fclose(lexFile);
}

void Merger::rawQuerySelector(){ //use the cout as output
  FILE * lexFile;
  lexFile = fopen(this->queryPool.c_str(), "r");
  if(lexFile == NULL){
    cout << "Problem! The file: " << this->queryPool << " could not be opened!" << endl; 
  }
  uint k = 0;
  char terms[ 1024 ];
  // string exception = "group home in new york";
  while(fgets(terms, sizeof terms, lexFile)!= NULL){
    if(k%3 == 0){
      string tmp(terms);
      // printf("%s\n", term);
      cout << tmp;
    }
    // if(k == 3000){
    if(k == 29){
      break;
    }
    k++;
  }
  fclose(lexFile);
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

void Merger::buildURLOnlyGraph(){
   this->loadUrls();
   this->getUrlNeighbors();
}
