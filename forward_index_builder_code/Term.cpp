#include "Term.h"

Term::Term() {
    this->termId = 0;
    this->frequency = 1;
}

Term::Term(uint termId) {
    this->termId = termId;
    this->frequency = 1;
}

Term::Term(uint termId, uint frequency) {
    this->termId = termId;
    this->frequency = frequency;
}
