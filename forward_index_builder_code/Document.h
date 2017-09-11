#ifndef __docid_reordering__Document__
#define __docid_reordering__Document__

#include <stdio.h>
#include <list>

#include "Term.h"
#include "globals.h"

using namespace std;
using namespace CONST;

class Document {
  private:
    uint docid;
    uint totalTerms;
    uint docLength; // total # term in document. Note these are not total unique terms in doc.
    //unsigned long long int totalFreq;
    vector<Term> terms;
    double score;
    string url;
    string baseUrl;
public:
   Document();
   Document(unsigned int);
   void addTerm(Term);
   const vector<Term> & getTerms() const;
   void setTermVec(vector<Term> inTerms);
   uint getTermCount();
   void setTermCount(const uint sz);
   double getScore();
   void serScore(double);
   void setDocId(unsigned int);
   uint getDocId();
	
   void setUrl(string url);
   const string getUrl() const;
	
   void setBaseUrl(string baseUrl);
   string getBaseUrl();

   void setDocLength(const uint inDocLen);
   uint getDocLength() const;
};

#endif /* defined(__docid_reordering__Document__) */
