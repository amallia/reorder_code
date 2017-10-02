#include "builder.h"
#include <sstream>
#include <algorithm>

template <typename Iterator>
void dumpToFile(const std::string& path, Iterator start, Iterator end) {
	FILE *fdo = fopen(path.c_str(),"w");
	// FILE *fdo = fopen(path.c_str(),"a+");
	if(! fdo) {
		std::cout << "Failed writing to " << path << " (Hint: create folder for path?)" << std::endl;
		return;
	}
	assert(fdo);
	for (; start !=end; ++start)
		if (fwrite(&(*start), sizeof(typename Iterator::value_type), 1, fdo) != 1)
			std::cout << "Failed writing to " << path << " (Hint: create folder for path?)" << std::endl;
	fclose(fdo);
}

/*sort the buffer vector by did then tid*/
bool SortByDidThenTid(const Posting p1, const Posting p2){
	if(p1.did < p2.did) {
		return true;
	}else if(p1.did > p2.did) {
		return false;
	}else{
		if(p1.tid < p2.tid) {
			return true;
		}else{
			return false;
		}
	}
}

/*constructor*/
Builder::Builder(const std::string index, const std::string lex, 
		const std::string doc_length, const int doc_num){
	fIndex = fopen(index.c_str(), "r");
	fLex = fopen(lex.c_str(), "r");
	fDocLength = fopen(doc_length.c_str(), "r");

	int total_terms;
	fread(&total_terms, sizeof(int), 1, fLex);
	std::cout << "There are " << total_terms << " terms in total\n";

	/*store listlength for each term*/
	listLengths = new unsigned int[ 4 * total_terms]; // read
	fread(listLengths, sizeof(int), 4 * total_terms, fLex);

	/*store offset into the index file for each term*/
	offsets = new uint[total_terms+1];
	offsets[0] = 0;
	for(int i=0; i<4*total_terms; i+=4)
		offsets[i/4 + 1] = (2*listLengths[i+1]*sizeof(uint))+offsets[i/4];

	/*store did length*/
	docLengths = new unsigned int[doc_num + 256];
	if( fread(docLengths, sizeof(int), doc_num, fDocLength) != (uint)doc_num )
		std::cout << "can not read doclength\n";

	for(int i = 0; i< 256; i++)
		docLengths[ i + doc_num] = 0;

	/*store lists*/
	tmpBuffer = new uint[doc_num * 2];

	/*keep track of the file*/
	file_counter = 0;
}

/*deconstructor*/
Builder::~Builder(){
	fclose(fIndex);
	fclose(fLex);
	fclose(fDocLength);

	delete[] listLengths;
	delete[] offsets; 
	delete[] docLengths;
	delete[] tmpBuffer;
}

/*get the (did, tid, freq) postings*/
void Builder::GetPostings(const uint term_id){
	assert(term_id == listLengths[4*(term_id-1)]);
	uint list_len = listLengths[4*(term_id-1) + 1];
	/*access the index sequentially, not jumping*/
	// std::cout << offsets[term_id-1] << std::endl;
	// fseek(fIndex,offsets[term_id-1],SEEK_SET); //jump to the right position
	if( (list_len*2)!= fread( tmpBuffer, sizeof(int), list_len*2, fIndex ))
				std::cout << "can not read index\n"; //<<listsize*2
	for(int i = 0; i < (int)list_len; i++){
		// Posting p(tmpBuffer[2*i] - 1, term_id, tmpBuffer[2*i + 1]);
		Posting p(tmpBuffer[2*i] - 1, term_id);
		this->global_buffer.push_back(p);
	}
}

/*generate postings from term_id a to term_id b*/
void Builder::GeneratePostings(const uint start, const uint end){
	/*jump to the right position once*/
	// std::cout << offsets[start-1] << std::endl;
	fseek(fIndex,offsets[start-1],SEEK_SET); 
	for(uint tid = start; tid <= end; tid++) {
		std::cout << tid << std::endl;
		GetPostings(tid);
		/*when the global buffer hit the threshold, write out*/
		if(this->global_buffer.size() >= BuilderData::kBufferSize) {
			sort(this->global_buffer.begin(), this->global_buffer.end(), SortByDidThenTid);
			WriteToDisk();
			this->global_buffer.clear();
		}
	}
	/*flush the buffer*/
	if(this->global_buffer.size() > 0){
		sort(this->global_buffer.begin(), this->global_buffer.end(), SortByDidThenTid);
		WriteToDisk();
	}
}

/*write buffer to disk*/
void Builder::WriteToDisk(){
	std::vector<uint> tmp;
	for(uint i=0; i<this->global_buffer.size(); i++){
		tmp.push_back(global_buffer[i].did);
		tmp.push_back(global_buffer[i].tid);
		// tmp.push_back(global_buffer[i].freq);
	}

	std::stringstream ss;
	ss << BuilderData::kBufferDir << this->file_counter;
	std::string filename = ss.str();
	dumpToFile(filename.c_str(), tmp.begin(), tmp.end());
	this->file_counter++;
}