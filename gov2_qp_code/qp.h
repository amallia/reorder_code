#include <iostream>
#include <fstream>
#include <vector>
using namespace std;
	
const string kQuery = "/home/qw376/reorder_data/query/cherry_queries";
const string kGov2Index = "/home/constantinos/Datasets/GOV2/Sorted/8_1.dat_sorted";
const string kGov2Lex = "/home/constantinos/Datasets/GOV2/8_1.inf";
const string kDocLenFile = "/home/constantinos/Datasets/GOV2/Sorted/doclen_file_sorted";
const string kSmallWordTable = "/home/qw376/reorder_data/lexicon/gov2_small_lex_1st";

class QpList{
public:
	int term_id;
	string term;
	int list_len;
	vector<int> dids;
	vector<int> freqs;
	QpList(string t);
};

class QpResult{
	vector<int> dids;
	vector<int> scores;
};

class Qp{
private:
	vector<vector<string>> queries;
	vector<QpResult> results;
public:
	void LoadIndex(string index_file);
	void LoadDocLength(string len_file);
	void LoadWordTable(string table_file);
	void LoadQuery(string query_file);
	void RunQuery();
	QpList LoadList(string term);
	QpResult AndQp(vector<QpList> lists);

};

