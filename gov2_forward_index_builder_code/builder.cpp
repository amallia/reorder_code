#include "builder.h"

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

/*get the (did, tid, freq) tuples*/
void Builder::GetPostings(const uint term_id, std::vector<Posting>& output){
	assert(term_id == listLengths[4*(term_id-1)]);
	uint list_len = listLengths[4*(term_id-1) + 1];
	std::cout << offsets[term_id-1] << std::endl;
	fseek(fIndex,offsets[term_id-1],SEEK_SET); //jump to the right position
	if( (list_len*2)!= fread( tmpBuffer, sizeof(int), list_len*2, fIndex ))
				std::cout << "can not read index\n"; //<<listsize*2
	std::vector<uint> dids;
	std::vector<uint> freqs;
	for(int i = 0; i < (int)list_len; i++){
		dids.push_back(tmpBuffer[2*i] - 1);
		freqs.push_back(tmpBuffer[2*i + 1]);
	}
}