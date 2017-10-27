#ifndef GOV2_TSP
#define GOV2_TSP
#include <iostream>
#include <vector>

typedef unsigned int uint;

namespace TspData{
	const std::string kForwardIndex = "/home/qw376/reorder_data/compressed_doc_index/raw_gov2_doc_index";
	const std::string kLex = "/home/qw376/reorder_data/compressed_doc_index/raw_gov2_doc_lex";
	const std::string kForwardIndexVB = "/home/qw376/reorder_data/compressed_doc_index/vb_gov2_doc_index";
	const std::string kLexVB = "/home/qw376/reorder_data/compressed_doc_index/vb_gov2_doc_lex";
	const std::string kLM = "/home/qw376/reorder_data/LM/ideal_lm_termIDs_1st";
	const std::string kOrdering = "/home/qw376/reorder_data/ordering/ideal_ordering";
	const uint kNumGov2Docs = 25205179;
	const uint kNumTerms = 32818983;
	const uint kNumNbs = 100;
	const long long kSize = 6279431929; 
	const long long kSizeVB = 7994112054;
}

class CmpList{
public:
	uint length;
	char* list;
	// CmpList(uint l) {
	// 	length = l;
	// }
	CmpList(uint l, char* c) {
		length = l;
		list = c;
	}
};

class WeightNode{
public:
	uint did;
	uint weight;
	WeightNode(uint d, uint w) {
		did = d;
		weight = w;
	}
};

class ProbNode{
public:
	uint tid;
	uint freq;
	ProbNode(uint t, uint f) {
		tid = t;
		freq = f;
	}
};

class Tsp{
private:
	uint last_scanned_did;
	uint* data;
	uint* lengths;
	long long* offsets;

public:
	Tsp();
	~Tsp();
	void Run(const std::string index, const std::string lex);
	void LoadLM(std::vector<std::vector<ProbNode> >& pair_probs);
	void LoadDocs(const std::string index,
		const std::string lex);
	void GetDoc(std::vector<uint>& terms, uint did);
	void GetNeighbors(std::vector<uint>& nbs, uint did);
	void UpdateEndDids(uint new_did, uint old_did,
		std::vector<uint>& end_dids);
	void GetNextCandidate(uint& old_did, uint& candidate, std::vector<bool>& visited,
		std::vector<uint>& ordering, std::vector<uint>& end_dids, std::vector<uint>& prev_dids,
		std::vector<std::vector<ProbNode> >& pair_probs);
	uint GetCandidateRandomly(std::vector<bool>& visited);
	void SetEndDids(std::vector<uint> terms, uint did,
		std::vector<uint>& end_dids, std::vector<uint>& prev_dids);
	void ResetEndDids(std::vector<uint> terms, 
		std::vector<uint>& end_dids, std::vector<uint>& prev_dids);
	uint GetMaxWeightNode(std::vector<WeightNode> weights);
	void WriteOrderingToDisk(std::vector<uint> ordering);
};

/*the reason use another class for this is 
to avoid branches, maybe there are better coding way*/
class TspVB{
private:
	char* data_vb;
	uint last_scanned_did;
	uint* lengths;
	uint* c_sizes;
	long long* offsets;
public:
	TspVB();
	~TspVB();
	void Run(const std::string index, const std::string lex);
	void LoadLM(std::vector<std::vector<ProbNode> >& pair_probs);
	void LoadDocs(const std::string index,
		const std::string lex);
	void GetDoc(std::vector<uint>& terms, uint did);
	void GetNeighbors(std::vector<uint>& nbs, uint did);
	void UpdateEndDids(uint new_did, uint old_did,
		std::vector<uint>& end_dids);
	void GetNextCandidate(uint& old_did, uint& candidate, std::vector<bool>& visited,
		std::vector<uint>& ordering, std::vector<uint>& end_dids, std::vector<uint>& prev_dids,
		std::vector<std::vector<ProbNode> >& pair_probs);
	uint GetCandidateRandomly(std::vector<bool>& visited);
	void SetEndDids(std::vector<uint> terms, uint did,
		std::vector<uint>& end_dids, std::vector<uint>& prev_dids);
	void ResetEndDids(std::vector<uint> terms, 
		std::vector<uint>& end_dids, std::vector<uint>& prev_dids);
	uint GetMaxWeightNode(std::vector<WeightNode> weights);
	void WriteOrderingToDisk(std::vector<uint> ordering);
};

#endif