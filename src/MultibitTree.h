// MultibitTree.h
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

#ifndef MULTIBITTREE_H
#define MULTIBITTREE_H

#include <assert.h>

#include "CLK.h"
#include "CLKSplitter.h"
#include "Misc.h"
#include "QueryResult.h"

#define LEAF_BIT 0x8000
#define BIT_MASK 0x7fff

// Objects of class MultibitTree contain a tree data structure for
// performing a fast similarity search. The underlying array of clks
// will be sorted according to the tree structure.

typedef unsigned short ushort;

template <class TT>
class MultibitTree {
	public:

	typedef TT P;
	typedef typename TT::S S;

	protected:

	int mCardinality;		// common cardinality for this tree
	int mLeafLimit;			// the maximum number of CLKs for which
					// no further sub-tree shall be calculated
	int mNBits;			// maximal size of CLKs in bits
	int64_t mLeafStart;		// start of tree in the array of CLKs
	int64_t mSize;			// length of tree in the array of CLKs
	int64_t mTreeSize;		// size of tree data structure
	int64_t mNodes;			// count of tree nodes
	ushort **mMatchBits;		// match-bit-list for each tree node
	ushort *mMatchBitsSize;		// total size of match bits for each tree node
	ushort *mMatchBitsZerosSize;	// size of zero match bits for each tree node
	int64_t *mLeftChild;		// left subtree node for each inner node
					// for leaf nodes this is reused for index into leaves list (start)
	int64_t *mRightChild;		// right subtree node for each inner node
					// for leaf nodes this is reused for index into leaves list (end)
	P **mLeaves;			// pointer to an array of CLKs

	CLKSplitter<P> mSplitter;	// helper class for splitting and match bit computation

	ushort *mMatchListZeros;	// temporary list for match bit computation
	ushort *mMatchListOnes;		// temporary list for match bit computation

	// recursively build a subtrees
	void buildNode(P *usedBits, int64_t leafStart, int64_t leafEnd);

	// recursively search sub tree by tanimoto similarity
	void internalSearch(QueryResult*, CLKTanimoto*, int64_t, int, int, int, int, S);
	// recursively search sub tree by hamming distance
	void internalSearch(QueryResult*, CLKHamming*, int64_t, int, int, int, S);

	public:

	// constructor
	// create a new MultibitTree from an array of CLKs
	MultibitTree(P **clks, int64_t leafStart, int64_t leafEnd, int nBits, int cardinality, int leafLimit);

	// destructor
	~MultibitTree();

	// perform a search for <query> that has <cardinality> filtered by <minSimilarity>
	inline void search(QueryResult*, TT*, int, S);

	// return tree size
	inline int64_t getSize() {
		return mSize;
	}
};

// constructor
// create a new MultibitTree from an array of CLKs
// clks		pointer to array of CLKs
// leafStart		cluster starting position in clks
// leafEnd		end of cluster
// nBits		maximal size of CLK in bits
// cardinality		cluster cardinality
// leafLimit		leaf limit for tree creation
template <class TT>
MultibitTree<TT>::MultibitTree(P **clks, int64_t leafStart, int64_t leafEnd, int nBits, int cardinality, int leafLimit) :
	mCardinality(cardinality),
	mLeafLimit(leafLimit),
	mNBits(nBits),
	mLeafStart(leafStart),
	mSize(leafEnd - leafStart),
	mLeaves(clks),
	mSplitter(clks, nBits)
{
	P *usedBits;

	// initialize member fields and allocate memory
	mTreeSize = MAX(2 * mSize - 1, 1);		// max size of binary-tree with "size" leaves
	mMatchBits = new ushort*[mTreeSize];
	mMatchBitsSize = new ushort[mTreeSize];
	mMatchBitsZerosSize = new ushort[mTreeSize];
	mLeftChild = new int64_t[mTreeSize];
	mRightChild = new int64_t[mTreeSize];

	mMatchListOnes = new ushort[nBits];
	mMatchListZeros = new ushort[nBits];
	usedBits = new P(nBits);
	mNodes = 0;

	// build tree
	buildNode(usedBits, leafStart, leafEnd);

	// free temporary data structures
	delete usedBits;
	delete[] mMatchListZeros;
	delete[] mMatchListOnes;
}

// destructor
// delete all used resources
template <class TT>
MultibitTree<TT>::~MultibitTree() {
	for (int64_t i = 0; i < mNodes; i++) {
		if ((mMatchBitsSize[i] & BIT_MASK) != 0) {
			delete[] mMatchBits[i];
		}
	}

	delete[] mMatchBits;
	delete[] mMatchBitsSize;
	delete[] mLeftChild;
	delete[] mRightChild;
}

// recursively build MultibitTree nodes and sub-nodes
template <class TT>
void MultibitTree<TT>::buildNode(P *usedBits, int64_t leafStart, int64_t leafEnd) {
	ushort listCountOnes;		// match bit counter for 1-bits
	ushort listCountZeros;		// match bit counter for 0-bits
	ushort listCount;		// match bit counter total;
	int64_t nLeaves;		// size of sub-tree range in leaves
	int64_t middle;			// index for sub-tree splitting
	int64_t thisNode;		// index of current node
	ushort *list;			// temporary match bit buffer
	P *clone;		// used bits clone for left sub-tree

	nLeaves = leafEnd - leafStart;

	thisNode = mNodes;
	mNodes++;

	assert(thisNode < mTreeSize);

	// count bits and find match-bits
	listCountOnes = 0;
	listCountZeros = 0;

	for (int i = 0; i < mNBits; i++) {
		mSplitter.resetCount(i);
		if (!usedBits->getBit(i)) {
			// for each unused bit count 1-bits within sub-tree range
			for (int leaf = leafStart; leaf < leafEnd; leaf++) {
				if (mLeaves[leaf]->getBit(i)) {
					mSplitter.incrementCount(i);
				}
			}

			if (mSplitter.getCount(i) == 0) {
				// if all bits are "0" store match-bit in zero-list
				mMatchListZeros[listCountZeros] = i;
				listCountZeros++;
				usedBits->setBit(i);
			} else if (mSplitter.getCount(i) == nLeaves) {
				// if all bits are "1" store match-bit in one-list
				mMatchListOnes[listCountOnes] = i;
				listCountOnes++;
				usedBits->setBit(i);
			}
		}
	}

	// copy match-bits in a well-sized persistent array
	listCount = listCountOnes + listCountZeros;

	if (listCount > 0) {
		list = new ushort[listCount];
		for (int i = 0; i < listCountZeros; i++) {
			list[i] = mMatchListZeros[i];
		}
		for (int i = 0; i < listCountOnes; i++) {
			list[listCountZeros + i] = mMatchListOnes[i];
		}
		mMatchBits[thisNode] = list;
	}

	mMatchBitsSize[thisNode] = listCount;
	mMatchBitsZerosSize[thisNode] = listCountZeros;

	// TDB: this could be checked earlier, if the deletion below could be omitted
	if (nLeaves < mLeafLimit) {
		middle = -1; // leaf
	} else {
		// TDB: this could be done earlier, if the deletion below could be omitted
		middle = mSplitter.splitRangeHalf(leafStart, leafEnd);
	}

	if (middle == -1) {
		// create leaf node
		mMatchBitsSize[thisNode] = LEAF_BIT;
		// TBD: original implementation requires a deletion of match-bits for leaf-nodes
		// check, if using them here may be more reasonable, too
		delete[] mMatchBits[thisNode];
		mLeftChild[thisNode] = leafStart;
		mRightChild[thisNode] = leafEnd;
	} else {
		// create inner node

		// copy to temporary clone of used bits for left sub-tree
		clone = new P(usedBits);

		// build left sub-tree
		mLeftChild[thisNode] = mNodes;
		buildNode(clone, leafStart, middle);

		// delete clone
		delete clone;

		// build right sub-tree
		mRightChild[thisNode] = mNodes;
		buildNode(usedBits, middle, leafEnd);
	}
}

// searching
// traverse the tree and visit only those sub-trees that don't surely underrun the tanimoto filter
template <>
void MultibitTree<CLKTanimoto>::internalSearch(QueryResult *result, CLKTanimoto *queryPrint, int64_t node, int commonXOR, int AB, int queryUnmatched, int treeUnmatched, float minTanimoto) {
	int size;
	ushort *matchBitIdx;
	size = mMatchBitsSize[node];
	if (size & LEAF_BIT) {
		// if this is a leaf-node check each leaf-CLK's tanimoto coefficient
		for (int64_t i = mLeftChild[node]; i < mRightChild[node]; i++) {
			CLKTanimoto *leaf = mLeaves[i];

			// increase statistic counter for XOR-hash estimation
			result->countXOR();

			// check XOR-hash estimation
			if (queryPrint->tanimotoXOR(leaf, AB) >= minTanimoto) {
				// increase statistic counter for tanimoto calculation
				result->countSim();

				// check exact tanimoto condition and add to results if matches
				float tanimoto = queryPrint->tanimoto(leaf);
				// check exact tanimoto condition
				if (tanimoto >= minTanimoto) {
					// add matching leaf to QueryResult
					result->add(queryPrint, leaf, tanimoto);
				}
			}
		}
	} else {
		// if this is an inner node estimate minimal tanimoto-coefficient
		// by evaluating the match-bits

		int countOnes = 0;
		int countZeros = 0;

		int sizeZeros =  mMatchBitsZerosSize[node];
		matchBitIdx = mMatchBits[node];

		// count differences: count 1-bits in query for all 0-bits in match-bits
		for (int i = 0; i < sizeZeros; i++) {
			countOnes += queryPrint->getBit(matchBitIdx[i]);
		}

		// count differences: count 0-bits in query for all 1-bits in match-bits
		for (int i = sizeZeros; i < size; i++) {
			countZeros += queryPrint->getBit(matchBitIdx[i]) ^ 1;
		}

		commonXOR += countZeros + countOnes;	// total differences
		queryUnmatched -= countOnes;		// subtract 1-bits of query-bits covered by match-bits
		treeUnmatched -= countZeros;		// subtract 1-bits of tree-bits  covered by match-bits

		// compute and compare minimal tanimoto-coefficient for this sub-tree
		if (((float) MIN(queryUnmatched, treeUnmatched)) / (commonXOR + MAX(queryUnmatched, treeUnmatched)) >= minTanimoto) {
			// analyse sub-trees
			internalSearch(result, queryPrint, mLeftChild[node], commonXOR, AB, queryUnmatched, treeUnmatched, minTanimoto);
			internalSearch(result, queryPrint, mRightChild[node], commonXOR, AB, queryUnmatched, treeUnmatched, minTanimoto);
		}
	}
}

// searching
// traverse the tree and visit only those sub-trees that don't surely overrun the hamming distance
template <>
void MultibitTree<CLKHamming>::internalSearch(QueryResult *result, CLKHamming *queryPrint, int64_t node, int AB, int queryUnmatched, int treeUnmatched, int maxHamming) {
	int size;
	ushort *matchBitIdx;

	size = mMatchBitsSize[node];

	if (size & LEAF_BIT) {
		// if this is a leaf-node check each leaf-CLK's hamming distance
		for (int64_t i = mLeftChild[node]; i < mRightChild[node]; i++) {
			CLKHamming *leaf = mLeaves[i];

			// increase statistic counter for XOR-hash estimation
			result->countXOR();

			// check XOR-hash estimation
			if (queryPrint->hammingXOR(leaf) <= maxHamming) {
				// increase statistic counter for hamming calculation
				result->countSim();

				// check exact hamming condition and add to results if matches
				int hamming = queryPrint->hamming(leaf);

				// check exact hamming condition
				if (hamming <= maxHamming) {
					// add matching leaf to QueryResult
					result->add(queryPrint, leaf, (float)(mNBits - hamming) / mNBits);
				}
			}
		}
	} else {
		// if this is an inner node estimate maximal hamming distance
		// by evaluating the match-bits

		int sizeZeros = mMatchBitsZerosSize[node];
		matchBitIdx = mMatchBits[node];

		// count differences: count 1-bits in query for all 0-bits in match-bits
		for (int i = 0; i < sizeZeros; i++) {
			queryUnmatched -= queryPrint->getBit(matchBitIdx[i]);
		}

		// count differences: count 0-bits in query for all 1-bits in match-bits
		for (int i = sizeZeros; i < size; i++) {
			treeUnmatched -= queryPrint->getBit(matchBitIdx[i]) ^ 1;
		}

		// compute and compare maximal hamming-distance for this sub-tree
		if (AB - 2 * MIN(queryUnmatched, treeUnmatched) <= maxHamming) {
			// analyse sub-trees
			internalSearch(result, queryPrint, mLeftChild[node], AB, queryUnmatched, treeUnmatched, maxHamming);
			internalSearch(result, queryPrint, mRightChild[node], AB, queryUnmatched, treeUnmatched, maxHamming);
		}
	}
}

// perform a search for <query> that has <cardinality> filtered by <minTanimotoSimilarity>
template <>
inline void MultibitTree<CLKTanimoto>::search(QueryResult *result, CLKTanimoto *queryPrint, int cardinality, float minTanimotoSimilarity) {
	internalSearch(result, queryPrint, 0, 0, cardinality + mCardinality, cardinality, mCardinality, minTanimotoSimilarity);
}

// perform a search for <query> that has <cardinality> filtered by <maxHammingDistance>
template <>
inline void MultibitTree<CLKHamming>::search(QueryResult *result, CLKHamming *queryPrint, int cardinality, int maxHammingDistance) {
	// printf("Search: %d %d\n", cardinality, minHamming);
	internalSearch(result, queryPrint, 0, cardinality + mCardinality, cardinality, mCardinality, maxHammingDistance);
}
#endif
