// CLKSorter.h
//
// Copyright (c) 2016
// Prof. Dr. Rainer Schnell
// Universitaet Duisburg-Essen
// Campus Duisburg
// Institut fuer Soziologie
// Lotharstr. 65
// 47057 Duisburg
//
// This file is part of the command line application "mbtSearch".
//
// "mbtSearch" is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// "mbtSearch" is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with "mbtSearch". If not, see <http://www.gnu.org/licenses/>.

#ifndef CLKSORTER_H
#define CLKSORTER_H

// objects of template class CLKSorter take an array of CLKs
// and sort the prints by cardinality, provide information of start and
// end positions of the cardinality clusters
template <class T>
class CLKSorter {
	public:

	CLKSorter () : mClusterPos(NULL), mClusterEnd(NULL) {}
	~CLKSorter() { clear(); }

	// get size of cluster i
	int64_t getSize(int i) { return mClusterEnd[i] - mClusterPos[i]; }
	// get start of cluster i
	int64_t getStart(int i) { return mClusterPos[i]; }
	// get end of cluster i
	int64_t getEnd(int i) { return mClusterEnd[i]; }

	// sort print array and save cluster information
	void cluster(T **prints, int64_t size, int nBits);

	private:

	int64_t *mClusterPos;
	int64_t *mClusterEnd;

	// reset cluster information
	void clear() {
		if (mClusterPos) { delete[] mClusterPos; }
		if (mClusterEnd) { delete[] mClusterEnd; }
		mClusterPos = NULL;
		mClusterEnd = NULL;
	}
};

// sort print array and save cluster information
template <class T>
void CLKSorter<T>::cluster(T **prints, int64_t size, int nBits) {
	clear();
	mClusterPos = new int64_t[nBits + 1];
	mClusterEnd  = new int64_t[nBits + 1];
	T *swap1, *swap2;	// helper pointer for sorting
	int card;			// helper variable for current cardinality

	// sort prints by cardinality
	// this can be done in linear time because we have a limited number of clusters

	// preliminary use clusterEnd array as counter
	for (int i = 0; i < (nBits + 1); i++) {
		mClusterEnd[i] = 0;
	}

	// run through all prints and count the occurence of all possible cardinalities
	for (int64_t i = 0; i < size; i++) {
		mClusterEnd[prints[i]->cardinality()]++;
	}

	// pre-calculate the sorting-destination position for each cardinality
	mClusterPos[0] = 0;

	for (int i = 1; i < (nBits + 1); i++) {
		mClusterPos[i] = mClusterPos[i - 1] + mClusterEnd[i - 1];
		mClusterEnd[i - 1] = mClusterPos[i - 1];
	}

	mClusterEnd[nBits] = mClusterPos[nBits];

	// swap each print that is not in the correct cardinality cluster
	for (int64_t i = 0; i < (size - 1); i++) {
		swap1 = prints[i];
		card = swap1->cardinality();

		// do swapping only if print is not in the right cluster
		if ((i < mClusterPos[card]) || (i >= mClusterEnd[card])) {
			// re-swap until the current destination equals i
			while (i != mClusterEnd[card]) {
				swap2 = prints[mClusterEnd[card]];	// get old print from destination
				prints[mClusterEnd[card]] = swap1;	// store new print to its destination
				mClusterEnd[card]++;			// increment cluster destination index
				swap1 = swap2;				// use old print as new candidate for swapping
				card = swap1->cardinality();
			}
			prints[i] = swap1;
			mClusterEnd[card]++;
		}
	}
}
#endif
