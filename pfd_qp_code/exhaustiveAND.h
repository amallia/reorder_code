#ifndef AND_H_
#define AND_H_

#include "PostingOriented_BMW.h"

class ExhaustiveAnd{
private:
	unsigned int* pages;

public:
	ExhaustiveAnd(unsigned int* pgs) : pages(pgs) {}
	void operator()(lptrArray& lps, const int topk, QpResult* r);
	void anotherAnd(lptrArray& lps, const int topk, QpResult* r);
};

#endif /* AND_H_ */
