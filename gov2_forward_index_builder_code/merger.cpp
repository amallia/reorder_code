#include "merger.h"
#include <iostream>
#include <cstring>

Merger::Merger(const std::string input_file_list){
	buffer = new char[MergerData::kMemSize];
	input_bufs = new Buffer[MergerData::kMaxFileNum];
	output_buf = new Buffer();

	fin_list = fopen(input_file_list.c_str(), "r");

	heap.arr = new int[MergerData::kMaxFileNum + 1];
	heap.cache = new char[MergerData::kMaxFileNum * MergerData::kRecSize];

	for(int i=0; i<MergerData::kMaxFileNum; i++) {
		fscanf(fin_list, "%s", filename);
		// if (feof(fin_list)) break;
		input_bufs[i].f = fopen(filename, "r");
		if(input_bufs[i].f == NULL) {
			std::cout << filename << " can not be opened\n";
		}
	}

	/* open output file (output is done by the last buffer in input_bufs) */
    // sprintf(filename, "%s%d", kOutputFile.c_str(), numFiles);
    output_buf->f = fopen64(MergerData::kOutputFile.c_str(), "w");

    /* assign buffer space (all buffers same space) and init to empty */
    for (int i = 0; i < MergerData::kMaxFileNum; i++) {
      input_bufs[i].buf = &(buffer[i*MergerData::kBufferSize]);
      input_bufs[i].curRec = 0;
      input_bufs[i].numRec = 0;
    }

    output_buf->buf = &(buffer[MergerData::kMaxFileNum * MergerData::kBufferSize]);
}

Merger::~Merger(){
	for(int i=0; i<MergerData::kMaxFileNum; i++) {
		fclose(input_bufs[i].f);
	}
	fclose(output_buf->f);
	fclose(fin_list);
	delete[] buffer;
	delete[] input_bufs;
	delete[] output_buf;
	delete[] heap.arr;
	delete[] heap.cache;
}

void Merger::merge(){
    /* initialize heap with first elements. Heap root is in heap[1] (not 0) */
    heap.size = MergerData::kMaxFileNum;
    /*heap[0] is not used, heap[1] stores the minmum, the reason is when do left shift, 
    start with 0 is more complex*/
    for(int i = 0; i <  MergerData::kMaxFileNum; i++) {
    	heap.arr[i+1] = nextRecord(i);
    }
    for(int i =  MergerData::kMaxFileNum; i > 0; i--) {
    	heapify(i);
    }

    /* now do the merge - ridiculously simple: do 2 steps until heap empty */
    while (heap.size > 0) {
      /* copy the record corresponding to the minimum to the output */
      writeRecord(heap.arr[1]); 

      /* replace minimum in heap by the next record from that file */
      if (nextRecord(heap.arr[1]) == -1)
        heap.arr[1] = heap.arr[heap.size--];     /* if EOF, shrink heap by 1 */
      if (heap.size > 1)  heapify(1);
    }

    /* flush output, add output file to list, close in/output files, and next */
    writeRecord(-1); 
}

/* get next record from input file into heap cache; return -1 if EOF */
int Merger::nextRecord(int i) {
  Buffer *b = &(input_bufs[i]);

  /* if buffer consumed, try to refill buffer with data */
  if (b->curRec == b->numRec)
    for (b->curRec = 0, b->numRec = 0; b->numRec * MergerData::kRecSize < MergerData::kBufferSize; b->numRec++)
    {
      fread(&(b->buf[b->numRec*MergerData::kRecSize]), MergerData::kRecSize, 1, b->f);
      if (feof(b->f))  break;
    }

  /* if buffer still empty, return -1; else copy next record into heap cache */
  if (b->numRec == 0)  return(-1);
  memcpy(heap.cache+i*MergerData::kRecSize, &(b->buf[b->curRec * MergerData::kRecSize]), MergerData::kRecSize);
  b->curRec++;
  return(i);
}

/* standard heapify on node i. Note that minimum is node 1. */
void Merger::heapify(int i) { 
  int s, t;
  s = i;
  while(1) {
    /* find minimum key value of current node and its two children */
    if (((i<<1) <= heap.size) && (KEY_ONE(i<<1) < KEY_ONE(i)))  s = i<<1;
    if (((i<<1) <= heap.size) && (KEY_ONE(i<<1) == KEY_ONE(i)) && (KEY_TWO(i<<1) < KEY_TWO(i)))  s = i<<1;
    if (((i<<1)+1 <= heap.size) && (KEY_ONE((i<<1)+1) < KEY_ONE(s)))  s = (i<<1)+1;
    if (((i<<1)+1 <= heap.size) && (KEY_ONE((i<<1)+1) == KEY_ONE(s)) && (KEY_TWO((i<<1)+1) < KEY_TWO(s)))  s = (i<<1)+1;

    /* if current is minimum, then done. Else swap with child and go down */
    if (s == i)  break;
    t = heap.arr[i];
    heap.arr[i] = heap.arr[s];
    heap.arr[s] = t;
    i = s;
  }
}

/* copy i-th record from heap cache to out buffer; write to disk if full */
/* If i==-1, then out buffer is just flushed to disk (must be last call) */
void Merger::writeRecord(int i) {
  int j;

  /* flush buffer if needed */
  if ((i == -1) || (output_buf->curRec*MergerData::kRecSize == MergerData::kBufferSize))
  { 
    for (j = 0; j < output_buf->curRec; j++)
      fwrite(&(output_buf->buf[j*MergerData::kRecSize]), MergerData::kRecSize, 1, output_buf->f);
    output_buf->curRec = 0;
  }

  if (i != -1)
    memcpy(&(output_buf->buf[(output_buf->curRec++)*MergerData::kRecSize]), 
    	heap.cache+i*MergerData::kRecSize, MergerData::kRecSize);
}