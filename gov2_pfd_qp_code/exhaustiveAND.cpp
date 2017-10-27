#include "exhaustiveAND.h"
#include "globals.h"
#include "profiling.h"
#include "utils.h"

void ExhaustiveAnd::GoodAnd(lptrArray& lps, const int topK, QpResult* res, long long& seek_counter) {
	int i, j, k;
	int frequency;
	float final_score, score, threshold;

	lps.sortByListLen();
	int cur_did = lps[0]->did;

	while(cur_did<CONSTS::MAXD) {
		i=0;
		for(; i<lps.size(); i++) {
			/*make sure every did in the list is equal or larger than the cur did*/
			if(lps[i]->did<cur_did) {
				lps[i]->nextGEQ(cur_did);
				seek_counter++;
			}
			/*move the cur did to the first list has a larger did*/
			if(lps[i]->did!=cur_did) {
				cur_did = lps[i]->did;
				break;
			}
		}

		/*if all the lists have the same did, start calculating score*/
		if(i==lps.size()) {
			// initialize final score
			k=0;
			final_score = 0;
			for (; k<lps.size(); k++){
				frequency = lps[k]->getFreq();
 				score = lps[k]->calcScore(frequency,pages[cur_did]);
 				final_score += score;
			}

			// if calculated score more than threshold, heapify
			threshold = res[topK-1].score;
			if (final_score > threshold) {
				//PROFILER(CONSTS::HEAPIFY);
				for (j = topK-2; (j >= 0) && (final_score > res[j].score); j--)
					res[j+1]=res[j];
				res[j+1].setR(cur_did,final_score);
			}

			/*move forward did on the short list*/
			lps[0]->nextGEQ(cur_did+1);
			seek_counter++;
			cur_did = lps[0]->did;
		}
	}
}

/*a wrong algorithm just for test purpose*/
void ExhaustiveAnd::operator() (lptrArray& lps, const int topK, QpResult* res) {
	// initial sorting by did
	lps.sort();

	int smallest_did = lps[0]->did;
	float final_score = 0;

	// initialize results heap
	for ( int i = 0; i < topK; ++i)  {
		res[i].did = -1;
		res[i].score = -1.0;
	}

	while(smallest_did < CONSTS::MAXD) {
		// set round's threshold
		const float threshold = res[topK-1].score;
		// initialize final score
		final_score = 0.0f;

		// evaluate all dids with did == smallest_did
		for (int i=0; i<lps.size(); ++i){
			if (lps[i]->did == smallest_did) {
				//PROFILER(CONSTS::EVAL);
				//PROFILER(CONSTS::GETFREQ);
				const float frequency = lps[i]->getFreq();
				const float score = lps[i]->calcScore(frequency,pages[smallest_did]);
				final_score += score;

				// move safely to the next did
				lps[i]->did = lps[i]->nextGEQ( smallest_did + 1 );
				//PROFILER(CONSTS::NEXTGEQ);
			}else{
				final_score = 0;
				break;
			}
		}

		// if calculated score more than threshold, heapify
		if (Fcompare(final_score, threshold)==1) {
			//PROFILER(CONSTS::HEAPIFY);
			int j;
			for (j = topK-2; (j >= 0) && (Fcompare(final_score, res[j].score)==1); j--)
				res[j+1]=res[j];
			res[j+1].setR(smallest_did,final_score);
		}

		// sort list by did
		lps.sort();

		// set new smallest did for the next round
		smallest_did = lps[0]->did;
	} //end while
}

/*for test purpose*/
void ExhaustiveAnd::anotherAnd(lptrArray& lps, const int topK, QpResult* res, long long& seek_counter) {
	lps.sortByListLen();

	for(int i=0; i<lps.size(); i++) {
		std::cout << lps[i]->term << " " << lps[i]->lengthOfList << " ";
	}
	std::cout << std::endl;

	// while(lps[1]->did < CONSTS::MAXD) {
	// 	std::cout << lps[1]->did << " ";
	// 	lps[1]->nextGEQ(lps[1]->did+1);
	// }
	// std::cout << std::endl;
}