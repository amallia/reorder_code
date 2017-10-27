#ifndef AND_H_
#define AND_H_

#include "PostingOriented_BMW.h"

class ExhaustiveAnd{
private:
	unsigned int* pages;

public:
	ExhaustiveAnd(unsigned int* pgs) : pages(pgs) {}
	void operator()(lptrArray& lps, const int topk, QpResult* r);
	void anotherAnd(lptrArray& lps, const int topk, QpResult* r, long long& seek_counter);
	void GoodAnd(lptrArray& lps, const int topk, QpResult* r, long long& seek_counter);
};

#endif /* AND_H_ */
