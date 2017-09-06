#include "pfor.h"
#include "CluewebReader.h"
#include "math.h"
#include "globals.h"

using namespace std;

bool sortPostingQP(const qpRunnerPosting& a, const qpRunnerPosting& b){
  return (a.did < b.did);
}

CluewebReader::CluewebReader(int _docn){

		docn = _docn; // CONSTS::MAXD Total # of documents in collection
		doclen = new unsigned int[docn + 256];
		for(int i = 0; i < docn + 256; i++){
			doclen[i] = 0;
		}

		termid_ = new unsigned int[this->numberOfTerms];
		listLen_ = new unsigned long[this->numberOfTerms];
		offset_ = new unsigned long[this->numberOfTerms];
		listLenBytes_ = new unsigned long[this->numberOfTerms];

		ifstream doc_lexikon_stream;
		doc_lexikon_stream.open(this->smallLex.c_str());

		string lexicon_line;
		string term;
		string termid_s;
		string listLen_s;
		string offset_s;
		string listLenBytes_s;
		int termid;
		long listLen;
		long offset;
		long listLenBytes;
		int linenum = 0;

		while (getline(doc_lexikon_stream, lexicon_line)){

//			cout<<"linenum: "<<linenum<<endl;
//			if(linenum == 10)
//				break;

			//term, first field
			string::iterator itr1 = lexicon_line.begin();
			string::iterator start1 = itr1;
			while(itr1 != lexicon_line.end() && !isspace(*itr1)){
				++itr1;
			}
			term = string(start1, itr1);
			term_.push_back(term);
			term_map[term] = linenum;
			this->termIDMap[linenum] = term;
			// cout<<"term: "<<term<<" ";
			//term

			 //termID, come after the 1st space
		  	 start1 = itr1+1;
		  	 itr1++;
		  	 while (!isspace(*itr1)) {
		  	 ++itr1;
		  	 }

		  	 termid_s = string(start1,itr1);
		  	 termid = atoi(termid_s.c_str());

		  	 // cout<<"termid: "<<termid<<" ";
		  	 termid_[linenum] = termid;
		  	 //termID

		  	 //listLen, come after the 2nd space
		  	 start1 = itr1+1;
		  	 itr1++;
		  	 while (!isspace(*itr1)) {
		  	 ++itr1;
		  	 }
		  	 listLen_s = string(start1, itr1);
		  	 listLen = atol(listLen_s.c_str());
			 // cout<<"listLen: "<<listLen<<" ";
			 listLen_[linenum] = listLen;
		  	 //listLen

			//offset, come after the 3th space
		  	start1 = itr1+1;
		  	itr1++;
		  	while (!isspace(*itr1)) {
		  	++itr1;
		  	}
		  	offset_s = string(start1, itr1);
		  	offset = atol(offset_s.c_str());
			// cout<<"offset: "<<offset<<" ";
			offset_[linenum] = offset;
		  	//offset

		  	//listLenBytes, come after the 4th space
		  	start1 = itr1+1;
		  	itr1++;
		  	while (!isspace(*itr1)) {
		  	   ++itr1;
		  	}
		  	listLenBytes_s = string(start1, itr1);
		  	listLenBytes = atol(listLenBytes_s.c_str());
		  	// cout<<"listLenBytes: "<<listLenBytes<<endl;
		  	listLenBytes_[linenum] = listLenBytes;
			//listLenBytes
			linenum++;
		}
		this->numTerms = linenum;
		cout<<"number of terms: "<< this->numTerms<<endl;
		doc_lexikon_stream.close();

//		for(int i=0; i<10; i++){
//			cout<<term_[i]<<" ";
//			cout<<termid_[i]<<" ";
//			cout<<listLen_[i]<<" ";
//			cout<<offset_[i]<<" ";
//			cout<<listLenBytes_[i]<<endl;
//		}

//		for (map<string, int>::iterator it = term_map.begin() ; it != term_map.end(); ++it){
//		        	cout<<it->first<<" "<<it->second<<endl;
//		}
}

void CluewebReader::loadDocLength(){

	// string input_source( "/data/constantinos/Index/documentLengths" );
	ifstream inputstream;
	inputstream.open(this->docLenFileDir.c_str());
	string curr_line_str;
	string doc_id_s;
	string doc_length_s;
	int doc_id;
	int doc_length;

	while (getline(inputstream, curr_line_str)) {
		string::iterator itr = curr_line_str.begin();
		string::iterator start = itr;

		//the first is doc id
		while(itr != curr_line_str.end() && !isspace(*itr)){
			++itr;
		}

		doc_id_s = string(start,itr);
//			  cout<<"doc_id: "<<doc_id_s<<endl;
		doc_id = atoi(doc_id_s.c_str());

		start = itr + 1;
		itr++;

		//the second is doc length
	    while (itr != curr_line_str.end()) {
			++itr;
	    }
	    doc_length_s = string(start,itr);
//			  cout<<"doc_length: "<<doc_length_s<<endl;
	    doc_length = atoi(doc_length_s.c_str());

	    doclen[doc_id] = doc_length;
	}

	inputstream.close();

	cout<<"doclength is loaded"<<endl;
	// for(int i = 0; i< 256; i++)
	// 	doclen[ i + docn ] = 0;
}

void CluewebReader::getDidTfs(RawIndexList& tList) {
	size_t tid = tList.termId;
	unsigned long listLen = listLen_[tid];
	unsigned long listLenBytes = listLenBytes_[tid];
	unsigned long offset = offset_[tid];

	compressed_list = new unsigned char[listLenBytes];
	uncompressed_list = new unsigned int[listLen*2];

	FILE* F_InvertedIndex = fopen(this->cluewebIndex.c_str(),"r");
	if( F_InvertedIndex == NULL ) cout << "Problem! The file: " << this->cluewebIndex << " could not be opened!" << endl;
	//  	  else cout << "Loading from file: " << InvertedIndex << endl;
	 fseek(F_InvertedIndex, offset, SEEK_SET);
	 if(fread(compressed_list, 1, listLenBytes, F_InvertedIndex)){
	 	decompressionVarBytes(compressed_list, uncompressed_list, listLen*2);
	 }
	 tList.doc_ids.reserve(listLen+CONSTS::BS);
	 tList.freq_s.reserve(listLen+CONSTS::BS);

	 for(unsigned int i=0; i<listLen; i++){
//		 cout<<"doc_id: "<<tList.doc_ids.back()+uncompressed_list[2*i]<<" ";
		 if(tList.doc_ids.size()!=0)
		 tList.doc_ids.push_back(tList.doc_ids.back()+uncompressed_list[2*i]); //take care of the d-gap
		 else
	     tList.doc_ids.push_back(uncompressed_list[2*i]);
//		 cout<<"freq: "<<uncompressed_list[2*i+1]<<endl;
		 tList.freq_s.push_back(uncompressed_list[2*i+1]);

	 }

	 tList.lengthOfList = listLen;
	 fclose (F_InvertedIndex);
	 delete compressed_list;
	 delete uncompressed_list;

	// cout<<"tid: "<<tid<<" listLen: "<<listLen<<endl;
}

void CluewebReader::applyNewOrderingToDidTfs(RawIndexList& tList){
  vector<qpRunnerPosting> postings;
  for(uint i = 0; i < tList.doc_ids.size(); i++){
    unordered_map<unsigned int, unsigned int>::iterator it;
    it = this->newOrderMap.find(tList.doc_ids[i]);
    uint newDid = 0;
    if(it!=this->newOrderMap.end()){
      newDid = this->newOrderMap[tList.doc_ids[i]];
    }
    else{
    	tList.lengthOfList--;
    	continue;
      // newDid = tList.doc_ids[i];
    }
    qpRunnerPosting p(newDid, tList.freq_s[i]);
    postings.push_back(p);
  }
  sort(postings.begin(), postings.end(), sortPostingQP);
  tList.doc_ids.clear();
  tList.freq_s.clear();
  for(uint i = 0; i < postings.size(); i++){
    tList.doc_ids.push_back(postings[i].did);
    tList.freq_s.push_back(postings[i].freq);
  }
}

void CluewebReader::updateLex(RawIndexList& tList){
	uint tid = tList.termId;
	listLen_[tid] = tList.lengthOfList;
}

void CluewebReader::writeOutNewLex(){
	FILE * lexDir = fopen(this->pfdLex.c_str(), "w");
	if( lexDir == NULL){
	    cout << "Problem! The file: " << this->pfdLex << " could not be opened!" << endl;
	    exit(0);
	}
	for(int i = 0; i < this->numTerms; ++i){
	    fprintf(lexDir, "%s %u %lu\n", this->termIDMap[i].c_str(), this->termid_[i], this->listLen_[i]);
	}
	fclose(lexDir);
    cout << "pfd Lex generated" << endl;
}

// Correct - unpadded score computation of BM25
// Usage: it loads to a RawIndexList structure (vectors of scores, docids, freqs)
// Input: term, term_id
// Output: RawIndexList structure
RawIndexList CluewebReader::loadRawList(const std::string& term, size_t wid) {
	// arguments: term, term_id
	BasicList basicList(term, wid);
	RawIndexList rawList(basicList);

	// Get docids, freqs for specific term
	this->getDidTfs(rawList);

	// apply new ordering
	this->applyNewOrderingToDidTfs(rawList);

	// update the listLength and generate the new lex
	this->updateLex(rawList);

	// Rank all docids
	rawList.rankWithBM25(doclen);

	// set unpadded list length
	rawList.unpadded_list_length = rawList.lengthOfList;

	// add dummy entries at the end to next multiple of CHUNK_SIZE
	// Docids = MAXD + 1, freqs = 1, scores = 0.0f
    for (size_t i = 0; (i == 0) || ((rawList.lengthOfList&(CONSTS::BS-1)) != 0); ++i)     {
    	rawList.doc_ids.push_back(CONSTS::MAXD + i);
    	rawList.freq_s.push_back(1);
    	rawList.scores.push_back(0.0f);
    	++rawList.lengthOfList;
    }
	return rawList;
}

int CluewebReader::decompressionVarBytes(unsigned char* input, unsigned int* output, int size){
    unsigned char* curr_byte = input;
    unsigned int n;
    for (int i = 0; i < size; ++i) {
	    unsigned char b = *curr_byte;
	    n = b & 0x7F;
//        cout<<"The first byte: "<<n<<endl;
//        printBinary(n);
//        cout<<endl;

	    while((b & 0x80) !=0){
	    	n = n << 7;
	    	++curr_byte;
	        b = *curr_byte;
	        n |= (b & 0x7F);
//	        cout<<"The following byte: "<<n<<endl;
//	        printBinary(n);
//	        cout<<endl;
	    }
    ++curr_byte;
    output[i] = n;
  }

  int num_bytes_consumed = (curr_byte - input);
  return (num_bytes_consumed >> 2) + ((num_bytes_consumed & 3) != 0 ? 1 : 0);

}

CluewebReader::~CluewebReader(void){
	delete[] doclen;
	delete[] termid_;
	delete[] listLen_;
	delete[] offset_;
	delete[] listLenBytes_;
}

// Print unsigned integer in binary format with spaces separating each byte.
void CluewebReader::printBinary(unsigned int num) {
	 int arr[32];
	  int i = 0;
	  while (i++ < 32 || num / 2 != 0) {
	    arr[i - 1] = num % 2;
	    num /= 2;
	  }

	  for (i = 31; i >= 0; i--) {
	    printf("%d", arr[i]);
	    if (i % 8 == 0)
	      printf(" ");
	  }
	  printf("\n");
}

void CluewebReader::loadNewOrdering(){
  FILE * lexFile;
  lexFile = fopen(this->ordering.c_str(), "r"); //random
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
  cout <<"did mapping map size: " << this->newOrderMap.size() <<endl;
  fclose(lexFile);
}

void CluewebReader::buildPFDIndex(){
	 this->loadDocLength();
	 this->loadNewOrdering();
	 for (size_t i=0; i< this->numTerms; i++){
	 // for (size_t i=0; i< 10; i++){
	 			   const string term = term_[i];
	 			   size_t termid = termid_[i];
	 			   RawIndexList Rlist = loadRawList(term_[i],i);
	 			   CompressedList Clist(Rlist);
	 			   Clist.serializeToDb(this->smallIndexDir, sql);
	 }
	 this->writeOutNewLex();
}
