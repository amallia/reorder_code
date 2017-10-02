#ifndef GOV2_MERGER
#define GOV2_MERGER

#include <iostream>

namespace MergerData{
	const int kRecSize = 8;
	const int kMaxFileNum = 3;
	const int kBufferSize = 1024*1024*10*kRecSize;
	const int kMemSize = kBufferSize*(kMaxFileNum+1);
	const std::string kInputList = "/home/qw376/reorder_data/tmp_buffer_data_gov2/input_list"; 
	const std::string kOutputFile = "/home/qw376/reorder_data/tmp_buffer_data_gov2/merged_index";
}

typedef struct {FILE *f; char* buf; int curRec; int numRec;} Buffer;
typedef struct {int *arr; char *cache; int size; } Heap;

#define KEY_ONE(z) (*(int *)(&(heap.cache[heap.arr[(z)]*MergerData::kRecSize])))
#define KEY_TWO(z) (*(int *)(&(heap.cache[heap.arr[(z)]*MergerData::kRecSize + sizeof(int)])))

class Merger{
private:
	Buffer *input_bufs;
	Buffer *output_buf;
	Heap heap;
	int num_files;
	char* buffer;
	char filename[1024];
	FILE* fin_list;

public:
	Merger(const std::string input_file_list);
	~Merger();
	void heapify(int i);
	void writeRecord(int i);
  	int nextRecord(int i);
  	void merge();

};

#endif