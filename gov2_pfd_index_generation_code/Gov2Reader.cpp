#include "pfor.h"
#include "Gov2Reader.h"
#include "math.h"
#include "globals.h"

using namespace std;

bool sortPostingQP(const qpRunnerPosting& a, const qpRunnerPosting& b){
	return (a.did < b.did);
}

Gov2Reader::~Gov2Reader(){
	fclose(findex);
	delete[] hold_buffer;
	delete[] doclen;
	delete[] lex_prefix;
	delete[] lex_buffer;
}

void Gov2Reader::LoadIndex(){
	/*load doc length*/
	LoadDocLength(CONSTS::MAXD);
	hold_buffer = new uint[CONSTS::MAXD*2];

	/*load lex*/
	FILE *flex = fopen(kGov2Lex.c_str(), "r");
	if(fread( &num_terms, sizeof(int), 1, flex)!=1) {
		std::cout << "trouble reading number of terms\n";
		exit(0); // first int contains the # of terms in the entire collection
	}
	std::cout << "num of terms: "<< num_terms << std::endl;

	lex_buffer = new uint[4*num_terms]; // read
	if(fread(lex_buffer, sizeof(int), 4*num_terms, flex)!= 4*num_terms) {
		std::cout << "trouble reading lex\n";
		exit(0);
	};
	//now we have all our sizes in an inf_buffer. Let's partial sum them
	lex_prefix = new uint[num_terms+1];
	lex_prefix[0] = 0;
	for(uint i=0; i<4*num_terms; i+=4)
		lex_prefix[i/4+1] = (2*lex_buffer[i+1]*sizeof(unsigned int)) + lex_prefix[i/4];
	fclose(flex);

	/*load small word file*/
	small_num_terms = 0;
	FILE *fileword = fopen(kSmallWordTable.c_str(), "r");
	if(fileword == NULL) {
		std::cout << "trouble reading small gov2 word table\n";
		exit(0);
	}
	char term[300];
	int tid;
	while(fscanf(fileword, "%s %d\n", term, &tid)!=EOF) {
		string termName(term);
		terms.push_back(termName);
		tids.push_back(tid);
		small_num_terms++;
	}
	fclose(fileword);

	/*open the index*/
	findex = fopen(kGov2Index.c_str(), "r");
}

void Gov2Reader::BuildPFDIndex(bool new_order){
	 LoadIndex();
	 if(new_order) {
	 	LoadOrdering(kOrdering);
	 }
	 cout << "Number of " << small_num_terms << " built for pdf index\n";
	 for (size_t i=0; i< small_num_terms; i++){
	 	RawIndexList Rlist = loadRawList(terms[i],tids[i],new_order);
	 	CompressedList Clist(Rlist);
	 	Clist.serializeToDb(kPFDIndex, sql);
	 }
	 WriteOutNewLex();
}

void Gov2Reader::WriteOutNewLex(){
	FILE * lexDir = fopen(kPFDLex.c_str(), "w");
	if(lexDir == NULL){
	    cout << "Problem! The file: " << kPFDLex << " could not be opened!" << endl;
	    exit(0);
	}
	// cout << kPFDLex << " opened\n";
	for(int i = 0; i < small_num_terms; ++i){
		// cout << terms[i] << " " << tids[i] << " " << list_len_map[tids[i]] << endl;
	    fprintf(lexDir, "%s %u %u\n", terms[i].c_str(), tids[i], list_len_map[tids[i]]);
	}
	fclose(lexDir);
    cout << "pfd Lex generated" << endl;
}

void Gov2Reader::LoadOrdering(const std::string order_file){
  FILE * file;
  file = fopen(order_file.c_str(), "r"); //random
  if( file == NULL) {
    cout << "Problem! The file: " << order_file << " could not be opened!" << endl;
    exit(0);
  }
  uint did = 0;
  uint newDid = 0;
  while(fscanf(file, "%d %d", &newDid, &did)!=EOF){
    // cout << did << " " << newDid << endl;
    order_map.insert(pair<int, int>(did, newDid));
  }
  cout <<"did mapping map size: " << order_map.size() <<endl;
  fclose(file);
}

// Correct - unpadded score computation of BM25
// Usage: it loads to a RawIndexList structure (vectors of scores, docids, freqs)
// Input: term, term_id
// Output: RawIndexList structure
RawIndexList Gov2Reader::loadRawList(const std::string& term,
	size_t wid, bool new_order) {
	// arguments: term, term_id
	BasicList basicList(term, wid);
	RawIndexList rawList(basicList);
	// Get docids, freqs for specific term
	getDidTfs(rawList);
	// apply new ordering
	if(new_order) {
		this->applyNewOrderingToDidTfs(rawList);
		// update the listLength and generate the new lex
		// this->updateLex(rawList);
	}

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

void Gov2Reader::LoadDocLength(int _docn){
	docn = _docn; // CONSTS::MAXD Total # of documents in collection
	doclen = new uint[docn + 256]();
	FILE* fdl = fopen(kDocLenFile.c_str(), "r"); // open doc_len file
	if(fdl == NULL) {
		std::cout << "Can not open " << kDocLenFile << std::endl;
		exit(0);
	}

	if( fread(doclen, sizeof(int), docn, fdl) != docn )
		std::cout << "can not read doclen\n";
	fclose(fdl);

	// for(int i = 0; i< 256; i++)
	// 	doclen[ i + docn ] = 0;

	cout<<"doclength is loaded"<<endl;
}

void Gov2Reader::getDidTfs(RawIndexList& tList) {
	uint tid = tList.termId;
	assert(tid == lex_buffer[4*(tid-1)]);
	uint listsize = lex_buffer[4*(tid-1) + 1];
	list_len_map[tid] = listsize;
	fseek(findex,lex_prefix[tid-1],SEEK_SET); //jump to the right position

	if( (listsize*2)!= fread( hold_buffer, sizeof(int), listsize*2, findex )) {
		cout << "can not read GOV2 index, abort\n";
		exit(0);
	}

	tList.doc_ids.reserve(listsize+CONSTS::BS);
	tList.freq_s.reserve(listsize+CONSTS::BS);

	for(int i = 0 ;i < listsize; i++){
		tList.doc_ids.push_back(hold_buffer[2*i] - 1);
		tList.freq_s.push_back(hold_buffer[2*i + 1]);
	}
	tList.lengthOfList = listsize;
}

void Gov2Reader::applyNewOrderingToDidTfs(RawIndexList& tList){
  vector<qpRunnerPosting> postings;
  int new_did;
  for(uint i = 0; i < tList.doc_ids.size(); i++){
    unordered_map<int, int>::iterator it;
    it = order_map.find(tList.doc_ids[i]);
    if(it!=order_map.end()){
      new_did= order_map[tList.doc_ids[i]];
    }
    else{
    	tList.lengthOfList--;
    	cout << "this should not happen for GOV2\n";
    	exit(0);
    	continue;
      // newDid = tList.doc_ids[i];
    }
    qpRunnerPosting p(new_did, tList.freq_s[i]);
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

// void Gov2Reader::updateLex(RawIndexList& tList){
// 	uint tid = tList.termId;
// 	list_len_map[tid] = tList.lengthOfList;
// }

int Gov2Reader::decompressionVarBytes(unsigned char* input, unsigned int* output, int size){
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

// Print unsigned integer in binary format with spaces separating each byte.
void Gov2Reader::printBinary(unsigned int num) {
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