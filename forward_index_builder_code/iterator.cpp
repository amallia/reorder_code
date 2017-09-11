// Includes.
#include <algorithm> // sort
#include <iostream>
#include <fstream>

#include "assert.h"

#include "globals.h"
#include "iterator.h"



// Helper methods.
void cmpList::PrintFirstIdsFreqs() {
  if (tids.size() > CONST::printNumDecoded && freqs.size() > CONST::printNumDecoded) {
    for (size_t i = 0; i < CONST::printNumDecoded; ++i) {
      cout << i << " did: " << tids[i] << " freq: " << freqs[i] << endl;
    }
  } else {
    cout << "There are less than " << CONST::printNumDecoded << " entries, so no print." << endl;
  }
}

bool cmpList::LoadFromIndex(ifstream &fH) {
  cmp.resize(lenBytes);
  buffer = reinterpret_cast<char*> (&cmp[0]);
 
  // Move to the appropriate offset and try to read the given # of bytes.
  fH.seekg( offset, ios::beg );
  if ( !fH.read(buffer, lenBytes) ) {
    cout << "Problem reading term's: " << url << " buffer of "
         << lenBytes << " bytes from index. Exiting." << endl;
    return false;
  }
  return true;
}

bool cmpList::LoadFromIndexSequentially(ifstream &fH) {
  cmp.resize(lenBytes);
  buffer = reinterpret_cast<char*> (&cmp[0]);
 
  // Move to the appropriate offset and try to read the given # of bytes.
  if ( !fH.read(buffer, lenBytes) ) {
    cout << "Problem reading term's: " << url << " buffer of "
         << lenBytes << " bytes from index. Exiting." << endl;
    return false;
  }
  return true;
}

bool cmpList::LoadFromPosIndex(ifstream &fH) {
  cmp.resize(lenBytes);
  buffer = reinterpret_cast<char*> (&cmp[0]);
 
  // Move to the appropriate offset and try to read the given # of bytes.
  fH.seekg( offset, ios::beg );
  if ( !fH.read(buffer, lenBytes) ) {
    cout << "Problem reading term's: " << url << " buffer of "
         << lenBytes << " bytes from positional index. Exiting." << endl;
    return false;
  }
  return true;
}

void cmpList::Print() {
  cout << "name: " << url << " id: " << id << " len: " << len << " totalOccur: "
       << totalOccur << " offset: " << offset << " lenBytes: " << lenBytes << endl;
}

void cmpList::PrefixSumDid() {
  assert(tids.size() > 0);
  for (size_t i = 1; i < tids.size(); ++i) {
    tids[i] += tids[i - 1];
  }
}

void cmpList::ComputeDgaps() {
  if (tids.size() == 1) return;
  for (int i = (int)tids.size() - 1; i > 0; --i) {
    tids[i] -= tids[i - 1];
  }
}

// Assumption: ids/freqs are decompressed in the corresponding vectors.
void cmpList::AddToDocIndex(vector<cmpList> &docIndex) {
  assert(docIndex.size() == CONST::numDocs);
  assert(tids.size() == freqs.size());
  for (uint i = 0; i < tids.size(); ++i) {
    uint docId = tids[i];
    assert(docId >= 0 && docId <= CONST::numDocs);

    // Compress each value by adding only the compressed value.
    // Assumption: No d-gap in ids.
    docIndex[ docId ].EncodeVarByteHigh( id );  // id represents the termId
    docIndex[ docId ].EncodeVarByteHigh( freqs[i] );
    ++docIndex[ docId ].len;  // increase by one the # postings in document.
    // Note that the lenBytes is increased by EncodeVarByteHigh().
  }
}

void cmpList::AddToDocPosIndex(vector<cmpList> &docPosIndex,
                               cmpList &posList) {
  // For each posting in original index in format (docid, freq), decompress # 
  // freqs positions from the given list that represents the positional index
  // and add them to the corresponding locations in the docPosIndex (after compressing them).
  assert(docPosIndex.size() == CONST::numDocs);
  assert(tids.size() == freqs.size());

  for (uint i = 0; i < tids.size(); ++i) {
    uint docId = tids[i];
    uint freq = freqs[i];
    assert(docId >= 0 && docId <= CONST::numDocs);

    // First, decompress # freqs position integers from the posList.
    // For each position, encode it and add it to the appropriate location.
    // Compress each value by adding only the compressed value.
    // Assumption: No d-gap in ids.
    for (uint numPos = 0; numPos < freq; ++numPos) {
      uint position = posList.DecodeVarByteHigh();
      docPosIndex[ docId ].EncodeVarByteHigh( id );  // id represents the termId
      docPosIndex[ docId ].EncodeVarByteHigh( position );
      ++docPosIndex[ docId ].len;  // increase by one the # postings in document.
      // Note that the lenBytes is increased by EncodeVarByteHigh().
    } // end of loop of adding the positions.
  } // end of loop over all postings of the original index
}

void cmpList::AddToDocLengthVec(vecUint &docLengths) {
  assert(len == tids.size());
  for (size_t i = 0; i < tids.size(); ++i) {
    uint docid = tids[i];
    ++docLengths[ docid ];
  }
}

// Sum all the frequencies of the document to obtain the document length and return it.
ulong cmpList::GetDocLength() {
  assert(len == freqs.size());
  ulong docLength = 0;
  for (size_t i = 0; i < freqs.size(); ++i) {
    docLength += freqs[i];
  }
  return docLength;
}

/////////////////////////////////////////////////////////////////////////
// ============ Varbyte with highest bit as continuation ============= //
/////////////////////////////////////////////////////////////////////////

uint cmpList::DecodeVarByte() {
  uint number = 0;
  uint tmpByte = 0x1;
  // First time we enter and we assume that the least significant bit is the cont. bit.
  while ((tmpByte & 0x1) == 0x1) {
    tmpByte = cmp[curByte];
    number = (number << 7) + ((cmp[curByte] >> 1) & 127);
    ++curByte;
  }
  return number;
}

void cmpList::EncodeVarByte(uint number) {
  unsigned char byteArr[CONST::numBytesInInt];
  bool started = false;
  int x = 0;

  for (x = 0; x < CONST::numBytesInInt; x++) {
    byteArr[x] = (number%128) << 1;
    number /= 128;
  }

  for (x = CONST::numBytesInInt - 1; x > 0; x--) {
    if (byteArr[x] != 0 || started == true) {
      started = true;
      byteArr[x] |= 0x1;
      cmp.push_back( byteArr[x] );
      ++lenBytes;
    }
  }
  byteArr[0] |= 0x0;
  cmp.push_back( byteArr[0] );
  ++lenBytes;
}

// Assumption: ids and frequencies are uncompressed in the corresponding vectors.
void cmpList::CompressVarByte() {
  assert(tids.size() == freqs.size());
  
  // Compute the d-gaps of the ids.
  ComputeDgaps();

  // For each posting, compress the id followed by the frequency.
  for (size_t i = 0; i < tids.size(); ++i) {
    EncodeVarByte(tids[i]);
    EncodeVarByte(freqs[i]);
  }
}

// Assuming the buffer is already filled with the appropriate data.
// Assuming that the ids/freq vectors are empty.
// Assuming we know the list length from the lexicon.
void cmpList::DecompressVarByte() {
  assert(tids.size() == freqs.size());
  assert(tids.size() == 0);
  
  // Allocate appropriate size.
  tids.reserve(len);
  freqs.reserve(len);
  for (ulong i = 0; i < len; ++i) {
    tids[i] = DecodeVarByte();
    freqs[i] = DecodeVarByte();
  }
  // Prefix Summing for Dids.
  PrefixSumDid();
}

/////////////////////////////////////////////////////////////////////////
// ============ Varbyte with highest bit as continuation ============= //
/////////////////////////////////////////////////////////////////////////

// Assuming the buffer is already filled with the appropriate data.
// Assuming that the ids/freq vectors are empty.
// Assuming we know the list length from the lexicon.
void cmpList::DecompressVarByteHigh() {
  assert(tids.size() == freqs.size());
  assert(tids.size() == 0);
  
  // Allocate appropriate size.
  tids.resize(len, 0);
  freqs.resize(len, 0);
  for (ulong i = 0; i < len; ++i) {
    tids[i] = DecodeVarByteHigh();
    freqs[i] = DecodeVarByteHigh();
  }
  // Prefix Summing for Dids.
  PrefixSumDid();
}

// The posting layout is only termIds and no prefix summing is required.
void cmpList::DecompressVarByteHighTermIds() {
  assert(tids.size() == 0);
  
  // Allocate appropriate size.
  tids.resize(len, 0);
  for (ulong i = 0; i < len; ++i) {
    tids[i] = DecodeVarByteHigh();
  }
}

void cmpList::DecompressVarByteHighWithoutPrefixSum() {
  assert(tids.size() == freqs.size());
  assert(tids.size() == 0);
  
  // Allocate appropriate size.
  tids.resize(len, 0);
  freqs.resize(len, 0);
  for (ulong i = 0; i < len; ++i) {
    tids[i] = DecodeVarByteHigh();
    freqs[i] = DecodeVarByteHigh();
  }
}

// The reason for this method is to fix the dgapped in positions.
// Currently, the posDocIndex is layout out as docid->(tid, pos), ..
// where the same tids have positions d-gapped. So, let say that
// there are 2 positions (102, 2) for tid = 2, after prefix sum,
// the positions should be 102, 104.
void cmpList::DecompressVarByteHighPosPrefixSum() {
  assert(tids.size() == freqs.size());
  assert(tids.size() == 0);

  // Allocate appropriate size.
  tids.resize(len, 0);
  freqs.resize(len, 0);
  for (ulong i = 0; i < len; ++i) {
    tids[i] = DecodeVarByteHigh();
    freqs[i] = DecodeVarByteHigh();
  }

  // Helper variables.
  uint prevTermId = CONST::numTerms + 1;
  uint position = 0;

  // Obtain the correct positions and store them in the frequency vector.
  for (ulong i = 0; i < len; ++i) {
    // In case, this posting has the same termId with the previous one,
    // prefix sum the positions to obtain the correct one.
    if (tids[i] == prevTermId) {
      position += freqs[i];
      freqs[i] = position;
    } else {  // This is the first time we see this termId.
      prevTermId = tids[i];
      position = freqs[i];
    }
  }
}

// Assumption: ids and frequencies are uncompressed in the corresponding vectors.
void cmpList::CompressVarByteHigh() {
  assert(tids.size() == freqs.size());

  // Compute the d-gaps for ids.
  ComputeDgaps();

  // For each posting, compress the id followed by the frequency.
  for (size_t i = 0; i < tids.size(); ++i) {
    EncodeVarByteHigh(tids[i]);
    EncodeVarByteHigh(freqs[i]);
  }
}

// Assumption: only termIds are encoded which are given in the vector.
void cmpList::CompressVarByteHighPosTermIds(vector<posting> &postings) {
  assert(tids.size() == 0);
 
  // For each id in each posting, encode it. 
  for (size_t i = 0; i < postings.size(); ++i) {
    EncodeVarByteHigh( postings[i].id );
    ++len; // increase the document length.
  }
  assert(len == postings.size());
}

// Given a vector of increasing docids, return the number of bytes required
// to encode the entire sequence of integers.
// Assumption: only docids are encoded.
long cmpList::GetVarByteHighBytes(vecUint &docids) {
  assert(tids.size() == 0);
 
  // For each docid, encode it. 
  for (size_t i = 0; i < docids.size(); ++i) {
    EncodeVarByteHigh( docids[i] );
    ++len; // increase the document length.
  }
  assert(len == docids.size());
  return lenBytes;
}

void cmpList::EncodeVarByteHigh(uint number) {
  unsigned char byteArr[CONST::numBytesInInt];
  bool started = false;
  int x = 0;

  for (x = 0; x < CONST::numBytesInInt; x++) {
    byteArr[x] = (number%128);
    number /= 128;
  }

  for (x = CONST::numBytesInInt - 1; x > 0; x--) {
    if (byteArr[x] != 0 || started == true) {
      started = true;
      byteArr[x] |= 128;
      cmp.push_back( byteArr[x] );
      ++lenBytes;
    }
  }
  cmp.push_back( byteArr[0] );
  ++lenBytes;
}

uint cmpList::DecodeVarByteHigh() {
  uint number = 0;
  uint tmpByte = 128;
  // First time we enter and we assume that the most significant bit is the cont. bit.
  while ((tmpByte & 128) == 128) {
    tmpByte = cmp[curByte];
    number = (number << 7) + (cmp[curByte] & 127);
    ++curByte;
  }
  return number;
}

void cmpList::DecodeVarByteOld() {
  uint decodedIds = 0;
  uint decodedFreqs = 0;
  uint cnt = 0;
  uint lastDidDecoded = 0;
  ulong cBytes = 0;
  uint decoded = 0;  // either did or frequency
  
  while (cBytes < lenBytes) {
    vector<unsigned char> result; // store multipliers - better way to do it ?
    while ( ( buffer[ cBytes ] & 128 ) > 0 ) { // check last bit - continuation bit
       result.push_back(  buffer[ cBytes ] & 127 ); // push back after clearing the last bit
       cBytes++; // next byte
    }
  
    // push last byte of current stream as is (since it does not have the last bit set)
    result.push_back( buffer[ cBytes ] ); 
    cBytes++;
  
    decoded = 0;
    uint resSz = (unsigned int)result.size() - 1;
    for (uint i=0; i<=resSz; i++) { // reconstruct number
      decoded += result[ i ] * (1<<(CONST::byteBits*(resSz-i)) ); // multiplier * (128^x)
    }
  
    // prefix sum for dids
    if (cnt%2 == 0) { // TODO avoid annoying branching please
      // add last decoded number (since it is d-gapped to construct the correct number
      decoded += lastDidDecoded;
      lastDidDecoded = decoded; // update last decoded number
      tids.push_back( decoded );
      //cout << "docid: " << decoded << endl;
      ++decodedIds;
    } else {
      freqs.push_back( decoded );
      //cout << "freqs: " << decoded << endl;
      ++decodedFreqs;
    }
    ++cnt;
 
  } // end of decoding of current stream.
  assert(decodedIds == decodedFreqs);
  assert(tids.size() == freqs.size());
  assert(tids.size() == len);

  // Reporting.
  // cout << "# ids decompressed: " << tids.size() << " and # freq: " << freqs.size() << endl;
  //PrintFirstIdsFreqs();
}

uint StrToUint(const string numberStr) {
    uint number = 0;
    istringstream(numberStr) >> number;
    return number;
}