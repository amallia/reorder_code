
#include <string>
#include <sstream>

class posting {
  public: 
         // members
         uint id;
         uint freqOrPos;

         // methods
         posting() {
           id = 0;
           freqOrPos = 0;
         }
         posting(const uint in_id,
                 const uint in_freqOrPos) {
           id = in_id;
           freqOrPos = in_freqOrPos;
         } 
         void Print();
};

bool compareId(const posting& i, const posting& j);

bool compareFreqOrPos(const posting& i, const posting& j);

bool compareDescendingFreqOrPos(const posting& i, const posting& j);

void sortById(vector<posting> &postings);

void sortByFreqOrPos(vector<posting> &postings);

void sortByDescendingFreqOrPos(vector<posting> &postings);

class basicList {
  public:
          // members
          string url;
          uint id;
          uint len;

          // method
          basicList() {
            url = "";
            id = 0;
            len = 0;
          }
          basicList(const string in_url,
                    const uint in_id, 
                    const uint in_len) {
            url = in_url;
            id = in_id;
            len = in_len;
          }
};

bool compareUrl(const basicList& i, const basicList& j);

void sortByUrl(vector<basicList> &basicListVec);

class cmpList : public basicList {
  public:
          // members
          vecUint tids;
          vecUint freqs;
          vecUchar cmp;
          char *buffer;
          ulong totalOccur;
          ulong offset;
          ulong lenBytes;
          ulong curByte;

          // constructor
          cmpList() {
            url = "";
            id = 0;
            len = 0;
            totalOccur = 0;
            offset = 0;
            lenBytes = 0;
            buffer = NULL;
            curByte = 0;
          }

          cmpList(const string in_url,
                  const uint in_id,
                  const uint in_len,
                  const ulong in_totalOccur,
                  const ulong in_offset,
                  const ulong in_lenBytes) {
            url = in_url;
            id = in_id;
            len = in_len;
            totalOccur = in_totalOccur;
            offset = in_offset;
            lenBytes = in_lenBytes;
            buffer = NULL;
            curByte = 0;
          }

          // destructor
          ~cmpList() {}

          // methods
          // Decompression of Varbyte (old way).
          void DecodeVarByteOld();

          // Helper
          void Print();
          void PrintFirstIdsFreqs();
          bool LoadFromIndex(ifstream &fH);
          bool LoadFromIndexSequentially(ifstream &fH);
          bool LoadFromPosIndex(ifstream &fH);
          void PrefixSumDid();
          void ComputeDgaps();
          void AddToDocIndex(vector<cmpList> &docIndex);
          void AddToDocPosIndex(vector<cmpList> &docPosIndex,
                                cmpList &posList);
          ulong GetDocLength();
          void AddToDocLengthVec(vecUint &docLengths);

          // Least significant bit is continuation bit.
          void EncodeVarByte(uint number);
          uint DecodeVarByte();
          void CompressVarByte();
          void DecompressVarByte();
 
          // Most significant bit is continuation bit.
          void EncodeVarByteHigh(uint number);
          uint DecodeVarByteHigh();
          void CompressVarByteHigh();
          void DecompressVarByteHigh();
          void DecompressVarByteHighWithoutPrefixSum();
          void DecompressVarByteHighPosPrefixSum();
          void CompressVarByteHighPosTermIds(vector<posting> &postings);
          void DecompressVarByteHighTermIds();
          long GetVarByteHighBytes(vecUint &docids);
};

uint StrToUint(const string numberStr);
