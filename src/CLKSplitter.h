// CLKSplitter.h
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

#ifndef CLK_SPLITTER_H
#define CLK_SPLITTER_H

#include "Misc.h"

// objects of template class CLKSplitter sort
// a range of a CLK array by the best dividing bit
template <class T>
class CLKSplitter {
	public:

	CLKSplitter(T **prints, int nBits) {
		mPrints = prints;
		mNBits = nBits;
		mCount = new int64_t[nBits];
	}

	~CLKSplitter() { delete[] mCount; };

	// increment counter for bit i
	void incrementCount(int i) { mCount[i]++; }
	// get current counter of bit i
	int64_t getCount(int i) { return mCount[i]; }
	// reset counter of bit i
	void resetCount(int i) { mCount[i] = 0; }
	// reset all counters
	void reset() { for (int i = 0; i < mNBits; i++) { mCount[i] = 0; } }
	// split range by best bit
	int64_t splitRangeHalf(int64_t leafStart, int64_t leafEnd);

	private:

	T **mPrints;
	int mNBits;
	int64_t *mCount;
};

// split range by best bit
template <class T>
int64_t CLKSplitter<T>::splitRangeHalf(int64_t leafStart, int64_t leafEnd) {
	int bestBit;		// best bit for sub-tree clustering
	int bestDist;		// best (lowest) distance between 1-bits and half of the clusters size
	int ones;		// number of 1-bits in the cluster
	int currentDist;	// current distance between 1-bits and half of the clusters size
	int64_t left, right;	// left and right sort index for 0/1-sorting
	int64_t half, size;	// half and total size of print range
	T *leaf;		// temporary pointer for swapping CLKs

	// find best bit for sub-tree clustering
	bestBit = -1;
	bestDist = mNBits;
	size = leafEnd - leafStart;
	half = size / 2;

	// computes best bit by lowest distance between 1-bits and half of the range size
	for (int i = 0; i < mNBits; i++) {
		ones = mCount[i];
		currentDist = ABS(ones - half);

		if ((ones != 0) && (ones != size) && (currentDist < bestDist)) {
			bestBit = i;
			bestDist = currentDist;
		}
	}
 
	if (bestBit == -1) {
		return -1; // no split => leaf
	}

	// sort and split sub-tree by bit
	// all CLKs with 0 at the bestBit-position are sorted to the left and
	// all CLKs with 1 at the bestBit-position are sorted to the right
	left = leafStart;
	right = leafEnd - 1;

	while (left < right) {
		if (!mPrints[left]->getBit(bestBit)) {
			left++;
			continue;
		}

		if (mPrints[right]->getBit(bestBit)) {
			right--;
			continue;
		}

		// swap leaves
		leaf = mPrints[left];
		mPrints[left] = mPrints[right];
		mPrints[right] = leaf;
	}

	return right;
}
#endif
