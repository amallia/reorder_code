#include "scanner.h"
#include <assert.h>

template <typename Iterator>
void WriteToFile(const std::string& path, Iterator start, Iterator end) {
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

int DecompressionVbytesInt(char* input, unsigned * output, int size){
    char* curr_byte = input;
    unsigned n;
    for (int i = 0; i < size; ++i) {
      char b = *curr_byte;
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

Scanner::Scanner(const std::string input_file) {
	fInput = fopen(input_file.c_str(), "r");
	if(fInput == NULL) {
		std::cout << input_file << " could not be opened\n";
	}
}

Scanner::~Scanner() {
	fclose(fInput);
}

void Scanner::WriteIndexRaw(std::string lex_path, std::string index_path){
  /*flush the buffer*/
  WriteToFile(index_path, this->list_raw.begin(), this->list_raw.end());
  /*write lexicon to disk*/
  FILE * lexDir = fopen(lex_path.c_str(), "w");
  if( lexDir == NULL){
     std::cout << "Problem! The file: " << lex_path << " could not be opened!" << std::endl;
     exit(0);
  }
  for(uint i = 0; i < dids.size(); ++i){
     fprintf(lexDir, "%u %u %lld\n", dids[i], lengths[i], offsets[i]);
  }
  fclose(lexDir);
    std::cout << "compressed gov2 doc index generated" << std::endl;
}

void Scanner::ScanRaw() {
	int did;
	int tid;
	long long newOffset = 0;
	pre_did = 0;
	num_of_postings = 0;
	while(num_of_postings < ScannerData::kTotalPostings) {
		fread(&did, sizeof(int), 1, fInput);
		fread(&tid, sizeof(int), 1, fInput);
		num_of_postings++;

		// std::cout << did << " " << tid << std::endl;
		if(did == pre_did) {
			tids.push_back(tid);
		} else {
			// for(uint i=0; i<tids.size(); i++){
			// 	std::cout << tids[i] << " ";
			// }
			// std:: cout << "\n";

			for(uint i=0; i< tids.size(); i++) {
				list_raw.push_back(tids[i]);
			}

			/*write to disk if the buffer hit a threshold*/
			// if(compressed_list.size() >= ScannerData::kThreshold) {
			// 	WriteToFile(ScannerData::kOutputIndex,
			// 	compressed_list.begin(), compressed_list.end());
			// 	compressed_list.clear();
			// }
		    std::cout << pre_did << " " << tids.size() << " "
		    << newOffset << " " << num_of_postings << std::endl;
		    dids.push_back(pre_did);
		    lengths.push_back(tids.size());
		    offsets.push_back(newOffset);
		    newOffset += tids.size();
			// for(uint i=0; i<tids.size(); i++){
			// 	std::cout << tids[i] << " ";
			// }
			// std:: cout << "\n";
			tids.clear();
			tids.push_back(tid);
			pre_did = did;
			// if(dids.size() == 2) break;
		}
	}
	/*write out the last did*/
	for(uint i=0; i< tids.size(); i++) {
		list_raw.push_back(tids[i]);
	}
    std::cout << did << " " << tids.size() << " " << tids.size() << " " << newOffset << std::endl;
    dids.push_back(did);
    lengths.push_back(tids.size());
    offsets.push_back(newOffset);

    /*write to disk, flush the remaining data to disk*/
	WriteIndexRaw(ScannerData::kOutputLex, ScannerData::kOutputIndex);
}

void ScannerVB::ScanVB() {
	int did;
	int tid;
	long long newOffset = 0;
	pre_did = 0;
	num_of_postings = 0;
	while(num_of_postings < ScannerData::kTotalPostings) {
		fread(&did, sizeof(int), 1, fInput);
		fread(&tid, sizeof(int), 1, fInput);
		num_of_postings++;
		// if(did < 25182426) {//25182427
		// 	continue;
		// }
		// std::cout << did << " " << tid << std::endl;
		// if(did > 25182430) {//25182429
		// 	break;
		// }
		// std::cout << did << " " << tid << std::endl;
		if(did == pre_did) {
			tids.push_back(tid);
		} else {
			// for(uint i=0; i<tids.size(); i++){
			// 	std::cout << tids[i] << " ";
			// }
			// std:: cout << "\n";
			for(uint i = tids.size()-1; i>0; i--) {
				tids[i] = tids[i] - tids[i-1];
			}

			uint c_size = compressionVbytes(tids);
			/*write to disk if the buffer hit a threshold*/
			// if(compressed_list.size() >= ScannerData::kThreshold) {
			// 	WriteToFile(ScannerData::kOutputIndex,
			// 	compressed_list.begin(), compressed_list.end());
			// 	compressed_list.clear();
			// }
		    std::cout << pre_did << " " << tids.size() << " " << c_size << " "
		    << newOffset << " " << num_of_postings << std::endl;
		    dids.push_back(pre_did);
		    lengths.push_back(tids.size());
		    c_sizes.push_back(c_size);
		    offsets.push_back(newOffset);
		    newOffset += c_size;
			// for(uint i=0; i<tids.size(); i++){
			// 	std::cout << tids[i] << " ";
			// }
			// std:: cout << "\n";
			tids.clear();
			tids.push_back(tid);
			pre_did = did;
			// if(dids.size() == 2) break;
		}
	}
	/*write out the last did*/
	for(uint i = tids.size()-1; i>0; i--) {
			tids[i] = tids[i] - tids[i-1];
	}
	uint c_size = compressionVbytes(tids);
    std::cout << did << " " << tids.size() << " " << c_size << " " << newOffset << std::endl;
    dids.push_back(did);
    lengths.push_back(tids.size());
    c_sizes.push_back(c_size);
    offsets.push_back(newOffset);

    /*write to disk, flush the remaining data to disk*/
	WriteIndex(ScannerData::kOutputLex, ScannerData::kOutputIndex);
}

void ScannerVB::WriteIndex(std::string lex_path, std::string index_path){
  /*flush the buffer*/
  WriteToFile(index_path, this->compressed_list.begin(), this->compressed_list.end());
  /*write lexicon to disk*/
  FILE * lexDir = fopen(lex_path.c_str(), "w");
  if( lexDir == NULL){
     std::cout << "Problem! The file: " << lex_path << " could not be opened!" << std::endl;
     exit(0);
  }
  for(uint i = 0; i < dids.size(); ++i){
     fprintf(lexDir, "%u %u %u %lld\n", dids[i], lengths[i], c_sizes[i], offsets[i]);
  }
  fclose(lexDir);
    std::cout << "compressed gov2 doc index generated" << std::endl;
}

uint ScannerVB::compressionVbytes(std::vector<uint> input){
   uint compressedSize = 0;
   for (uint i = 0; i < input.size(); ++i){
          unsigned char byteArr[ScannerData::kIntSize];
          bool started = false;
          int x = 0;

          for (x = 0; x < ScannerData::kIntSize; x++) {
             byteArr[x] = (input[i]%128);
             input[i] /= 128;
          }

          for (x = ScannerData::kIntSize - 1; x > 0; x--) {
            if (byteArr[x] != 0 || started == true) {
              started = true;
              byteArr[x] |= 128;
              this->compressed_list.push_back(byteArr[x]);
              compressedSize++;
            }
          }
          this->compressed_list.push_back(byteArr[0]);
          compressedSize++;
   }
   return compressedSize;
}

ScannerVB::ScannerVB(const std::string input_file) {
	fInput = fopen(input_file.c_str(), "r");
	if(fInput == NULL) {
		std::cout << input_file << " could not be opened\n";
	}
}

ScannerVB::~ScannerVB() {
	fclose(fInput);
}

void ReaderVB::LoadLexVB(const std::string lex_path,
	uint* lengths, uint* c_sizes, long long* offsets) {
	std::cout << "loading lex from " << lex_path << std::endl;
	FILE* fLex = fopen(lex_path.c_str(), "r");
	if(fLex == NULL) {
		std::cout << "could not open " << lex_path << std::endl;
		exit(0);
	}
	uint did;
  	uint length;
  	uint c_size;
  	long long offset;
	while(fscanf(fLex, "%u %u %u %lld\n", &did, &length, &c_size, &offset)!=EOF) {
    	// std::cout << did << " " << length << " " << c_size << " " << offset << std::endl;
    	lengths[did] = length;
    	c_sizes[did] = c_size;
    	offsets[did] = offset;
	}
	fclose(fLex);
}

void ReaderVB::ReadDocumentVB(uint did, const std::string index_path,
	uint* lengths, uint* c_sizes, long long* offsets) {
	uint length = lengths[did];
	uint c_size = c_sizes[did];
	long long offset = offsets[did];
	if(length == 0){
		std::cout << "document length is zero, nothing to read\n";
		return;
	}
	FILE* fIndex = fopen(index_path.c_str(), "r");
	if(fIndex == NULL) {
		std::cout << index_path << " can not be opened\n";
		exit(0);
	}
	fseek(fIndex, offset, SEEK_SET);
	char* cp = new char[c_size];
    fread(cp, 1, c_size, fIndex);
    uint* uncomp_list = new unsigned[length];
  	DecompressionVbytesInt(cp, uncomp_list, length);
  	std::vector<uint> terms;
  	terms.push_back(uncomp_list[0]);
  	std::cout << "Reading did: " << did << " length: " <<
	length << " c_size: " << c_size << " offset: " << offset << std::endl;
  	std::cout << uncomp_list[0]<<" ";
  	for(uint i = 1; i < length; ++i){
    	std::cout << terms.back() + uncomp_list[i]<<" ";
    	terms.push_back( terms.back() + uncomp_list[i] );
  	}
  	std::cout << std::endl;
}

void Reader::LoadLexRaw(const std::string lex_path,
	uint* lengths, long long* offsets) {
	std::cout << "loading lex from " << lex_path << std::endl;
	FILE* fLex = fopen(lex_path.c_str(), "r");
	if(fLex == NULL) {
		std::cout << "could not open " << lex_path << std::endl;
		exit(0);
	}
	uint did;
  	uint length;
  	long long offset;
	while(fscanf(fLex, "%u %u %lld\n", &did, &length, &offset)!=EOF) {
    	// std::cout << did << " " << length << " " << offset << std::endl;
    	lengths[did] = length;
    	offsets[did] = offset;
	}
	fclose(fLex);
}

void Reader::ReadDocumentRaw(uint did, const std::string index_path,
	uint* lengths, long long* offsets) {
	uint length = lengths[did];
	long long offset = offsets[did];
	FILE* fIndex = fopen(index_path.c_str(), "r");
	if(fIndex == NULL) {
		std::cout << index_path << " can not be opened\n";
		exit(0);
	}
	fseek(fIndex, offset*4, SEEK_SET);
	std::cout << "Reading did: " << did << " length: " <<
	length << " offset: " << offset << std::endl;
	uint* cp = new uint[length];
    fread(cp, sizeof(uint), length, fIndex);
  	for(uint i = 0; i < length; ++i){
    	std::cout << cp[i] << " ";
  	}
  	std::cout << std::endl;
  	delete[] cp;
}