#include "tsp.h"
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>


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

Tsp::Tsp() {
	last_scanned_did=0;
}

Tsp::~Tsp() {
	delete[] data;
	delete[] lengths;
	delete[] offsets;
}

void Tsp::Run(const std::string index,
	const std::string lex) {
	std::vector<uint> ordering(TspData::kNumGov2Docs, 0);
	std::vector<bool> visited(TspData::kNumGov2Docs, false);
	std::vector<uint> prev_dids(TspData::kNumTerms, TspData::kNumGov2Docs);
	std::vector<uint> end_dids(TspData::kNumTerms, TspData::kNumGov2Docs);
	uint source = 1; //25182428;
	uint uv_with_neighbor;
	uint uv_no_neighor;
	/*the old did is starting from 0, new did also need to start from zero*/
	uint did_selected = 0;
	ordering[0] = source;
	visited[source] = true;

	/*load LM*/
	std::vector<std::vector<ProbNode> > pair_probs;
	LoadLM(pair_probs);

	/*load documents*/
	std::cout << "Loading documents...\n";
	LoadDocs(index, lex);
	std::cout << "Loading documents done...\n";
	// std::cout << data[215] << " " << data[216] << std::endl;
	// std::vector<uint> terms;
	// GetDoc(terms, 25205178);
	// std::cout << terms.size() << std::endl;
	// for(uint i=0; i<terms.size(); i++) {
	// 	std::cout << terms[i] << " ";
	// }
	// std::cout << std::endl;
	// exit(0);
	/*do tsp*/
	while(did_selected < TspData::kNumGov2Docs) {
		std::cout << "new did: " << did_selected << " old did: " << source << std::endl;
		UpdateEndDids(did_selected, source, end_dids);
		GetNextCandidate(did_selected, source, visited, 
		ordering, end_dids, prev_dids, pair_probs);
		// if(did_selected == 10) break;
	}
	/*write ordering to disk*/
	WriteOrderingToDisk(ordering);
}

/*load the language model*/
void Tsp::LoadLM(std::vector<std::vector<ProbNode> >& pair_probs){
	pair_probs.resize(TspData::kNumTerms);
	for(uint i = 0; i < TspData::kNumTerms; i++){//termId start from 1, so use <=
	    std::vector<ProbNode> t;
	    pair_probs.push_back(t);
  	}
  	FILE* fH = fopen(TspData::kLM.c_str(), "r");
	if ( fH == NULL ) std::cout << TspData::kLM << " file could not be opened" << std::endl;
	else std::cout << "Loading sorted pair probabilities from file " << TspData::kLM << std::endl;
	uint tid1;
	uint tid2;
	uint freq;

	while( fscanf( fH,"%u %u %u\n", &tid1, &tid2, &freq) != EOF ){
	   ProbNode p(tid2, freq);
	   pair_probs[tid1].push_back(p);
	}
	fclose(fH);
}

/*get the next candidate according to the break points*/
void Tsp::GetNextCandidate(uint& new_did, uint& old_did,
	std::vector<bool>& visited, std::vector<uint>& ordering, std::vector<uint>& end_dids,
	std::vector<uint>& prev_dids, std::vector<std::vector<ProbNode> >& pair_probs) {

	std::vector<uint> nbs;
	GetNeighbors(nbs, old_did);
	// std::cout << nbs.size() << std::endl;
	// for(uint i=0; i< nbs.size(); i++) {
	// 	std::cout << nbs[i] << " "; 
	// }
	// std::cout << std::endl;
	std::vector<WeightNode> weights;
	for(uint i=0; i<nbs.size(); i++) {
		if(visited[nbs[i]]) continue;
		int total_weight = 0;
		uint cur_did = nbs[i];
		std::vector<uint> terms;
		GetDoc(terms, cur_did);
		/*set the last and prev end dids*/
		SetEndDids(terms, cur_did, end_dids, prev_dids);
		for(uint j=0; j<terms.size(); j++) {
			uint num_pairs = pair_probs[terms[j]-1].size();
			uint prev_did_t1 = prev_dids[terms[j]-1];
			for(uint k=0; k<num_pairs; k++) {
				ProbNode p = pair_probs[terms[j]-1][k];
				/*find out which did term2 recently appeared in*/
				uint end_did_t2 = end_dids[p.tid];
				/*if last did of t2 is between last and second last did of t1, then penalize*/
				if(end_did_t2<cur_did && end_did_t2>prev_did_t1) {
					total_weight+=p.freq;
				}
			}
		}
		/*reset the last and prev end dids*/
		ResetEndDids(terms, end_dids, prev_dids);
		WeightNode node(cur_did,total_weight);
		weights.push_back(node);
	}
	if(weights.size()!=0) {
		old_did = GetMaxWeightNode(weights);
	}else{
		/*no available neighbor from this one, then we random select one with neighbors*/
		old_did = GetCandidateRandomly(visited);
	}
	new_did++;
	ordering[new_did] = old_did;
	visited[old_did] = true;
}

/*use old_did to get the terms inside of it, change the end did of the terms to new_did*/
void Tsp::UpdateEndDids(uint new_did, uint old_did,
		std::vector<uint>& end_dids){
	std::vector<uint> terms;
	GetDoc(terms, old_did);
	// for(uint i=0; i<terms.size(); i++) {
	// 	std::cout << terms[i] << " ";
	// }
	// std::cout << std::endl;
	for(uint i=0; i<terms.size(); i++) {
    	end_dids[terms[i]-1] = new_did;
  	}
}

void Tsp::WriteOrderingToDisk(std::vector<uint> ordering){
	FILE* output = fopen(TspData::kOrdering.c_str(), "w");
	if(output == NULL) {
		std::cout << TspData::kOrdering << " can not be opened\n";
	}
	for(int i = 0; i < ordering.size(); ++i){
    	fprintf(output, "%u %u\n", i, ordering[i]);
  	}
  	fclose(output);
  	std::cout << "Writing out new reordering done \n";
}

void Tsp::SetEndDids(std::vector<uint> terms, uint did,
	std::vector<uint>& end_dids, std::vector<uint>& prev_dids){
	for(uint i=0; i<terms.size(); i++) {
		prev_dids[terms[i]-1] = end_dids[terms[i]-1];
		end_dids[terms[i]-1] = did;
	}
}

void Tsp::ResetEndDids(std::vector<uint> terms,
	std::vector<uint>& end_dids, std::vector<uint>& prev_dids){
	for(uint i=0; i<terms.size(); i++) {
		end_dids[terms[i]-1] = prev_dids[terms[i]-1];
	}
}

/*sort the buffer vector by did then tid*/
bool SortByWeight(const WeightNode w1, const WeightNode w2){
	return (w1.weight < w2.weight);
}

uint Tsp::GetMaxWeightNode(std::vector<WeightNode> weights){
	std::sort(weights.begin(), weights.end(), SortByWeight);
	// for(uint i=0; i<weights.size(); i++) {
	// 	std::cout << weights[i].did << " " << weights[i].weight << " ";
	// }
	// std::cout << std::endl;
	return weights[0].did;
}

uint Tsp::GetCandidateRandomly(std::vector<bool>& visited){
	/*first try to randomly select from nodes with neighbors*/
	for(uint i=last_scanned_did; i<TspData::kNumGov2Docs; i++) {
		if(!visited[i]) {
			last_scanned_did = i+1;
			return i;
		}
	}
}

void Tsp::GetNeighbors(std::vector<uint>& nbs, uint did) {
	uint nb_top = TspData::kNumNbs/2;
	uint nb_bot = TspData::kNumNbs/2;
	int bot = did - TspData::kNumNbs/2;
	int top = did + TspData::kNumNbs/2;

	if(bot < 0) {
		nb_bot = did;
		nb_top = TspData::kNumNbs - nb_bot;
	}else if(top > TspData::kNumGov2Docs) {
		nb_top = TspData::kNumGov2Docs - did - 1;
		nb_bot = TspData::kNumNbs - nb_top;
	}
	// std::cout << nb_top << " " << nb_bot << std::endl;
	int k = 1;
	int j = 1;
	while(k+j <= TspData::kNumNbs) {
		if(k<=nb_top) {
			nbs.push_back(did + k);
			k++;
		}
		if(j<=nb_bot) {
			nbs.push_back(did - j);
			j++;
		}
	}
}

void Tsp::GetDoc(std::vector<uint> &terms, uint did) {
	/*the did in gov2 lex is starting from 0, so just it for vector index*/
	// std::cout << "did: " << did << " length: " << lengths[did] << " offset: " << offsets[did] << std::endl;
	for(long long i=offsets[did]; i<offsets[did]+lengths[did]; i++) {
		terms.push_back(data[i]);
	}
}

void Tsp::LoadDocs(const std::string index, const std::string lex) {
	FILE* fIndex = fopen(index.c_str(), "r");
	if(fIndex == NULL) {
		std::cout << "could not open " << index << std::endl;
		exit(0);
	}

	/*load index*/
	int data_size;
	data = new uint[TspData::kSize];
	fread(data, sizeof(uint), TspData::kSize, fIndex);
	fclose(fIndex);

	FILE* fLex = fopen(lex.c_str(), "r");
	if(fLex == NULL) {
		std::cout << "could not open " << lex << std::endl;
		exit(0);
	}

	/*load lex*/
	lengths = new uint[TspData::kNumGov2Docs]();
	offsets = new long long[TspData::kNumGov2Docs]();
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


/*methods for TSP VB*/

TspVB::TspVB() {
	last_scanned_did=0;
}

TspVB::~TspVB() {
	delete[] data_vb;
	delete[] lengths;
	delete[] c_sizes;
	delete[] offsets;
}

void TspVB::Run(const std::string index,
	const std::string lex) {
	std::vector<uint> ordering(TspData::kNumGov2Docs, 0);
	std::vector<bool> visited(TspData::kNumGov2Docs, false);
	std::vector<uint> prev_dids(TspData::kNumTerms, TspData::kNumGov2Docs);
	std::vector<uint> end_dids(TspData::kNumTerms, TspData::kNumGov2Docs);
	uint source = 1; //25182428;
	uint uv_with_neighbor;
	uint uv_no_neighor;
	/*the old did is starting from 0, new did also need to start from zero*/
	uint did_selected = 0;
	ordering[0] = source;
	visited[source] = true;

	/*load LM*/
	std::vector<std::vector<ProbNode> > pair_probs;
	LoadLM(pair_probs);

	/*load documents*/
	std::cout << "Loading documents...\n";
	LoadDocs(index, lex);
	std::cout << "Loading documents done...\n";
	// std::cout << data[215] << " " << data[216] << std::endl;
	// std::vector<uint> terms;
	// GetDoc(terms, 25205178);
	// std::cout << terms.size() << std::endl;
	// for(uint i=0; i<terms.size(); i++) {
	// 	std::cout << terms[i] << " ";
	// }
	// std::cout << std::endl;
	// exit(0);
	/*do tsp*/
	while(did_selected < TspData::kNumGov2Docs) {
		std::cout << "new did: " << did_selected << " old did: " << source << std::endl;
		UpdateEndDids(did_selected, source, end_dids);
		GetNextCandidate(did_selected, source, visited, 
		ordering, end_dids, prev_dids, pair_probs);
		// if(did_selected == 10) break;
	}
	/*write ordering to disk*/
	WriteOrderingToDisk(ordering);
}

void TspVB::GetDoc(std::vector<uint> &terms, uint did) {
	/*the did in gov2 lex is starting from 0, so just use it for vector index*/
	// std::cout << "did: " << did << " length: " << lengths[did] 
	// <<" c_size: " << c_sizes[did] << " offset: " << offsets[did] << std::endl;
	uint length = lengths[did];
	if(length == 0) return;
	uint c_size = c_sizes[did];
	long long offset = offsets[did]; 
	char* c_data = new char[c_size];
	int k = 0;
	for(long long i=offset; i<offset+c_size; i++) {
		c_data[k++] = data_vb[i];
	}
	uint* uc_data = new unsigned[length];
	DecompressionVbytesInt(c_data, uc_data, length);
	terms.push_back(uc_data[0]);
	// std::cout << did << " content:\n";
  	// std::cout << uc_data[0]<<" ";
	for(uint i = 1; i < length; ++i){
    	// std::cout << terms.back() + uc_data[i]<<" ";
    	terms.push_back( terms.back() + uc_data[i] );
  	}
  	delete[] c_data;
  	delete[] uc_data;
}

void TspVB::LoadDocs(const std::string index, const std::string lex) {
	FILE* fIndex = fopen(index.c_str(), "r");
	if(fIndex == NULL) {
		std::cout << "could not open " << index << std::endl;
		exit(0);
	}

	/*load index*/
	int data_size;
 	data_vb = new char[TspData::kSizeVB];
 	fread(data_vb, 1, TspData::kSizeVB, fIndex);
	fclose(fIndex);

	FILE* fLex = fopen(lex.c_str(), "r");
	if(fLex == NULL) {
		std::cout << "could not open " << lex << std::endl;
		exit(0);
	}

	/*load lex*/
	lengths = new uint[TspData::kNumGov2Docs]();
	c_sizes = new uint[TspData::kNumGov2Docs]();
	offsets = new long long[TspData::kNumGov2Docs]();
	uint did;
  	uint length;
  	uint c_size;
  	long long offset;
	while(fscanf(fLex, "%u %u %u %lld\n", &did, &length, &c_size, &offset)!=EOF) {
		// std::cout << did << " " << length << " " << c_size << " " << offset << std::endl;
		lengths[did] = length;
    	offsets[did] = offset;
    	c_sizes[did] = c_size;
	}
	fclose(fLex);
}

/*load the language model*/
void TspVB::LoadLM(std::vector<std::vector<ProbNode> >& pair_probs){
	pair_probs.resize(TspData::kNumTerms);
	for(uint i = 0; i < TspData::kNumTerms; i++){//termId start from 1, so use <=
	    std::vector<ProbNode> t;
	    pair_probs.push_back(t);
  	}
  	FILE* fH = fopen(TspData::kLM.c_str(), "r");
	if ( fH == NULL ) std::cout << TspData::kLM << " file could not be opened" << std::endl;
	else std::cout << "Loading sorted pair probabilities from file " << TspData::kLM << std::endl;
	uint tid1;
	uint tid2;
	uint freq;

	while( fscanf( fH,"%u %u %u\n", &tid1, &tid2, &freq) != EOF ){
	   ProbNode p(tid2, freq);
	   pair_probs[tid1].push_back(p);
	}
	fclose(fH);
}

uint TspVB::GetMaxWeightNode(std::vector<WeightNode> weights){
	std::sort(weights.begin(), weights.end(), SortByWeight);
	// for(uint i=0; i<weights.size(); i++) {
	// 	std::cout << weights[i].did << " " << weights[i].weight << " ";
	// }
	// std::cout << std::endl;
	return weights[0].did;
}

uint TspVB::GetCandidateRandomly(std::vector<bool>& visited){
	/*first try to randomly select from nodes with neighbors*/
	for(uint i=last_scanned_did; i<TspData::kNumGov2Docs; i++) {
		if(!visited[i]) {
			last_scanned_did = i+1;
			return i;
		}
	}
}

void TspVB::GetNeighbors(std::vector<uint>& nbs, uint did) {
	uint nb_top = TspData::kNumNbs/2;
	uint nb_bot = TspData::kNumNbs/2;
	int bot = did - TspData::kNumNbs/2;
	int top = did + TspData::kNumNbs/2;

	if(bot < 0) {
		nb_bot = did;
		nb_top = TspData::kNumNbs - nb_bot;
	}else if(top > TspData::kNumGov2Docs) {
		nb_top = TspData::kNumGov2Docs - did - 1;
		nb_bot = TspData::kNumNbs - nb_top;
	}
	// std::cout << nb_top << " " << nb_bot << std::endl;
	int k = 1;
	int j = 1;
	while(k+j <= TspData::kNumNbs) {
		if(k<=nb_top) {
			nbs.push_back(did + k);
			k++;
		}
		if(j<=nb_bot) {
			nbs.push_back(did - j);
			j++;
		}
	}
}

/*use old_did to get the terms inside of it, change the end did of the terms to new_did*/
void TspVB::UpdateEndDids(uint new_did, uint old_did,
		std::vector<uint>& end_dids){
	std::vector<uint> terms;
	GetDoc(terms, old_did);
	// for(uint i=0; i<terms.size(); i++) {
	// 	std::cout << terms[i] << " ";
	// }
	// std::cout << std::endl;
	for(uint i=0; i<terms.size(); i++) {
    	end_dids[terms[i]-1] = new_did;
  	}
}

void TspVB::WriteOrderingToDisk(std::vector<uint> ordering){
	FILE* output = fopen(TspData::kOrdering.c_str(), "w");
	if(output == NULL) {
		std::cout << TspData::kOrdering << " can not be opened\n";
	}
	for(int i = 0; i < ordering.size(); ++i){
    	fprintf(output, "%u %u\n", i, ordering[i]);
  	}
  	fclose(output);
  	std::cout << "Writing out new reordering done \n";
}

void TspVB::SetEndDids(std::vector<uint> terms, uint did,
	std::vector<uint>& end_dids, std::vector<uint>& prev_dids){
	for(uint i=0; i<terms.size(); i++) {
		prev_dids[terms[i]-1] = end_dids[terms[i]-1];
		end_dids[terms[i]-1] = did;
	}
}

void TspVB::ResetEndDids(std::vector<uint> terms,
	std::vector<uint>& end_dids, std::vector<uint>& prev_dids){
	for(uint i=0; i<terms.size(); i++) {
		end_dids[terms[i]-1] = prev_dids[terms[i]-1];
	}
}

/*get the next candidate according to the break points*/
void TspVB::GetNextCandidate(uint& new_did, uint& old_did,
	std::vector<bool>& visited, std::vector<uint>& ordering, std::vector<uint>& end_dids,
	std::vector<uint>& prev_dids, std::vector<std::vector<ProbNode> >& pair_probs) {

	std::vector<uint> nbs;
	GetNeighbors(nbs, old_did);
	// std::cout << nbs.size() << std::endl;
	// for(uint i=0; i< nbs.size(); i++) {
	// 	std::cout << nbs[i] << " "; 
	// }
	// std::cout << std::endl;
	std::vector<WeightNode> weights;
	for(uint i=0; i<nbs.size(); i++) {
		if(visited[nbs[i]]) continue;
		int total_weight = 0;
		uint cur_did = nbs[i];
		std::vector<uint> terms;
		GetDoc(terms, cur_did);
		/*set the last and prev end dids*/
		SetEndDids(terms, cur_did, end_dids, prev_dids);
		for(uint j=0; j<terms.size(); j++) {
			uint num_pairs = pair_probs[terms[j]-1].size();
			uint prev_did_t1 = prev_dids[terms[j]-1];
			for(uint k=0; k<num_pairs; k++) {
				ProbNode p = pair_probs[terms[j]-1][k];
				/*find out which did term2 recently appeared in*/
				uint end_did_t2 = end_dids[p.tid];
				/*if last did of t2 is between last and second last did of t1, then penalize*/
				if(end_did_t2<cur_did && end_did_t2>prev_did_t1) {
					total_weight+=p.freq;
				}
			}
		}
		/*reset the last and prev end dids*/
		ResetEndDids(terms, end_dids, prev_dids);
		WeightNode node(cur_did,total_weight);
		weights.push_back(node);
	}
	if(weights.size()!=0) {
		old_did = GetMaxWeightNode(weights);
	}else{
		/*no available neighbor from this one, then we random select one with neighbors*/
		old_did = GetCandidateRandomly(visited);
	}
	new_did++;
	ordering[new_did] = old_did;
	visited[old_did] = true;
}