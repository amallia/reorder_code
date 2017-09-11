#include "Document.h"

Document::Document() {
	
}

void Document::setTermVec(vector<Term> inTerms) {
  terms = inTerms;
}

Document::Document(unsigned int docid) {
	this->docid = docid;
}

void Document::addTerm(Term t) {
    this->terms.push_back(t);
}


const vector<Term> & Document::getTerms() const{
    return this->terms;
}

unsigned int Document::getTermCount() {
	return totalTerms;
}

void Document::setTermCount(const uint sz) {
   totalTerms = sz;
}

double Document::getScore() {
	return this->score;
}

void Document::serScore(double score) {
	this->score = score;
}

void Document::setDocId(unsigned int docId) {
	this->docid = docId;
}

unsigned int Document::getDocId() {
	return  this->docid;
}

void Document::setUrl(string url) {
	this->url = url;
}

const string Document::getUrl() const{
	return this->url;
}

void Document::setBaseUrl(string url) {
	this->baseUrl = url;
}

string Document::getBaseUrl() {
	return this->baseUrl;
}

uint Document::getDocLength() const {
	return this->docLength;
}

void Document::setDocLength(const uint inDocLen) {
  this->docLength = inDocLen;
}
