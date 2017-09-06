#ifndef BLOCKGENS_H_
#define BLOCKGENS_H_

#include "globals.h"
#include "utils.h"
#include <cmath>

typedef vecUInt SingleBlock;
typedef std::vector<SingleBlock>::const_iterator BlocksIterator;

template<typename T>
T getMaxIndirect(const SingleBlock& blockInds, const std::vector<T>& vals) {
	T m = vals[ *(blockInds.cbegin())];
	for( auto it = blockInds.cbegin(); it != blockInds.cend(); ++it)
		if(m < vals[(*it)] )
			m = vals[(*it)];
	return m;
}

template<typename T>
std::vector<T> getVaues(const SingleBlock& blockInds, const std::vector<T>& vals) {
	std::vector<T> tmp;
	tmp.reserve(blockInds.size());
	for(SingleBlock::const_iterator it = blockInds.cbegin(); it != blockInds.end(); ++it)
		tmp.push_back(vals[*it]);
	return tmp;
}

class AbsBlocksGen : public std::vector<SingleBlock> {
public:
	AbsBlocksGen() {}
	virtual ~AbsBlocksGen() {}
	BlocksIterator getIterator() { return this->cbegin(); }
	BlocksIterator getEnd() { return this->cend(); }
	virtual void generateBlocks() {}

	static unsigned int getMaxDidOfBlock(const SingleBlock& block, const vecUInt& dids ) { return getMaxIndirect(block,dids); }
	static float getMaxScoreOfBlock(const SingleBlock& block, const std::vector<float>& scores) { return getMaxIndirect(block,scores); }
	static unsigned int getMaxDidOfBlock(const BlocksIterator& block, const vecUInt& dids ) { return getMaxIndirect(*block,dids); }
	static float getMaxScoreOfBlock(const BlocksIterator& block, const std::vector<float>& scores) { return getMaxIndirect(*block,scores); }
};

class PostingsBlocker : public AbsBlocksGen {
	size_t BS;
	size_t length;

public:
	PostingsBlocker(size_t len, size_t bs = CONSTS::BS ) : BS(bs), length(len){}
	virtual void generateBlocks();
};

#endif /* BLOCKGENS_H_ */

