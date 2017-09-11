#include "exhaustiveAND.h"
#include "globals.h"
#include "profiling.h"
#include "utils.h"

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


void ExhaustiveAnd::anotherAnd(lptrArray& lps, const int topK, QpResult* res, long long& seek_counter) {
	// initial sorting by did
	lps.sortByListLen();

	int candidateDid = lps[0]->did;
	int nextCandidate = lps[0]->did;
	float final_score = 0;
	float threshold = 0;
	float frequency = 0;
	float score = 0;
	int i, j, k;

	while(candidateDid < CONSTS::MAXD) {
		i = 0;
		// evaluate all dids with did == smallest_did
		for (; i<lps.size(); ++i){
			if (lps[i]->did < candidateDid) {
				lps[i]->nextGEQ( candidateDid );
				seek_counter++;
			}
			if(lps[i]->did != candidateDid){
				nextCandidate = lps[i]->did;
				break;
			}
		}

		if(i == lps.size()){
			// initialize final score
			final_score = 0.0f;
			for (; k<lps.size(); ++k){
				frequency = lps[k]->getFreq();
 				score = lps[k]->calcScore(frequency,pages[candidateDid]);
 				final_score += score;
			}
			// if calculated score more than threshold, heapify
			threshold = res[topK-1].score;
			if ( final_score > threshold ) {
				//PROFILER(CONSTS::HEAPIFY);
				for (j = topK-2; (j >= 0) && ( final_score > res[j].score ); j--)
					res[j+1]=res[j];
				res[j+1].setR(candidateDid,final_score);
			}
	      	candidateDid ++;
	    } else {  // move the shortest list in the did where the mismatch occured.
	      	candidateDid = nextCandidate;
	    }
	} //end while

}


// void ExhaustiveAnd::anotherAnd(lptrArray& lps, const int topK, QpResult* res) {
// 	// initial sorting by did
// 	lps.sortByListLen();

// 	int candidateDid = lps[0]->did;
// 	uint nextCandidate = lps[0]->did;
// 	float final_score = 0;

// 	while(candidateDid < CONSTS::MAXD) {
// 		// set round's threshold
// 		const float threshold = res[topK-1].score;
// 		// initialize final score
// 		final_score = 0.0f;

// 		int i = 0;
// 		// evaluate all dids with did == smallest_did
// 		for (; i<lps.size(); ++i){
// 			if (lps[i]->did < candidateDid) {
// 				lps[i]->did = lps[i]->nextGEQ( candidateDid );
// 			}
// 			if(lps[i]->did == candidateDid){
// 				const float frequency = lps[i]->getFreq();
// 				const float score = lps[i]->calcScore(frequency,pages[candidateDid]);
// 				final_score += score;
// 			}else{
// 				nextCandidate = lps[i]->did;
// 				break;
// 			}
// 		}

// 		// if calculated score more than threshold, heapify
// 		if (Fcompare(final_score, threshold)==1) {
// 			//PROFILER(CONSTS::HEAPIFY);
// 			int j;
// 			for (j = topK-2; (j >= 0) && (Fcompare(final_score, res[j].score)==1); j--)
// 				res[j+1]=res[j];
// 			res[j+1].setR(candidateDid,final_score);
// 		}

// 		// set new smallest did for the next round
// 	    if (i == lps.size()) { // in case of alignment, move the shortest list by one.
// 	      candidateDid = lps[0]->nextGEQ(candidateDid + 1);
// 	    } else {  // move the shortest list in the did where the mismatch occured.
// 	      candidateDid = lps[0]->nextGEQ(nextCandidate);
// 	    }
// 	} //end while

// }