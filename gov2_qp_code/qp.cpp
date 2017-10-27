#include "qp.h"

/*functions of QpList*/
QpList::QpList(string t){
	term = t;
}

/*functions of QP*/
void Qp::RunQuery(){
	for(int k=0; k<(int)queries.size(); k++) {
		vector<QpList> lists;
		for(int i=0; i<(int)queries[k].size(); i++) {
			QpList l(queries[k][i]);
			lists.push_back(l);
		}
		QpResult res = AndQp(lists);
		results.push_back(res);
	}
}

void Qp::LoadQuery(string query_file) {

} 

QpResult Qp::AndQp(vector<QpList> lists) {
	QpResult res;

	return res;
}