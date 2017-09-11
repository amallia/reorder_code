#ifndef __docid_reordering__Term__
#define __docid_reordering__Term__

#include <stdio.h>

typedef unsigned int uint;

class Term {
public:
    Term();
    Term(uint termId);
    Term(uint termId, uint frequency);
    uint termId;
    uint frequency;
};

#endif /* defined(__docid_reordering__Term__) */
