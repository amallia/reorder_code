#ifdef gov2_merger
#define gov2_merger

#include <iostream>

typedef struct {FILE *f; char* buf; int curRec; int numRec;} Buffer;
typedef struct {int *arr; char *cache; int size; } Heap;
#define KEY_ONE(z) (*(int *)(&(heap.cache[heap.arr[(z)]*recSize])))
#define KEY_TWO(z) (*(int *)(&(heap.cache[heap.arr[(z)]*recSize + sizeof(int)])))

const int kRecSize = 8;
const int kMemSize = 1024*1024;

class Merger{
private:
	Buffer *io_bufs;
	Heap heap;
	int num_files;
	char* buffer;
	char file_name[1024];

public:
	Merger(const std::string input_file_list);
	~Merger();
	void heapify(int i);
	void writeRecord(Buffer *b, int i);
  	int nextRecord(int i);

};

#endif