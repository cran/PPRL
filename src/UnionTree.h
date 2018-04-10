// UnionTree.h
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

#ifndef UNIONTREE_H
#define UNIONTREE_H

#include <assert.h>

#include "CLK.h"
#include "CLKSplitter.h"
#include "Misc.h"
#include "QueryResult.h"

// Objects of class UnionTree contain a tree data structure for
// performing a fast similarity search. The underlying array of prints
// will be sorted according to the tree structure.

template <class TT>
class UnionTree {
	public:

	typedef TT P;
	typedef typename TT::S S;

	// protected:

	int mCardinality;		// common cardinality for this tree
	int mLeafLimit;			// the maximum number of CLKs for which
					// no further sub-tree shall be calculated
	int mNBits;			// maximal size of CLKs in bits
	int64_t mLeafStart;		// start of tree in the array of CLKs
	int64_t mSize;			// length of tree in the array of CLKs
	int64_t mTreeSize;		// size of tree data structure
	int64_t mNodes;			// count of tree nodes
	P **mUnion;			// union print for each inner node
	char *mIsLeaf;			// leaf flag for each inner node
	int64_t *mLeftChild;		// left subtree node for each inner node
					// for leaf nodes this is reused for index into leaves list (start)
	int64_t *mRightChild;		// right subtree node for each inner node
					// for leaf nodes this is reused for index into leaves list (end)
	P **mLeaves;			// pointer to an array of CLKs


	CLKSplitter<P> mSplitter;	// helper class for splitting and match bit computation

	// recursively build a subtrees
	void buildNode(int64_t leafStart, int64_t leafEnd);

	// recursively search sub tree by tanimoto similarity
	void internalSearch(QueryResult*, CLKTanimoto*, int64_t, int, S);
	// recursively search sub tree by hamming distance
	void internalSearch(QueryResult*, CLKHamming*, int64_t, int, S);

	public:

	// constructor
	// create a new UnionTree from an array of CLKs
	UnionTree(P **prints, int64_t leafStart, int64_t leafEnd, int nBits, int cardinality, int leafLimit);

	// destructor
	~UnionTree();

	// perform a search for <query> that has <cardinality> filtered by <minSimilarity>
	inline void search(QueryResult*, TT*, int, S);

	// return tree size
	inline int64_t getSize() {
		return mSize;
	}
};

// constructor
// create a new UnionTree from an array of CLKs
// prints		pointer to array of CLKs
// leafStart		cluster starting position in prints
// leafEnd		end of cluster
// nBits		maximal size of CLK in bits
// cardinality		cluster cardinality
// leafLimit		leaf limit for tree creation
template <class TT>
UnionTree<TT>::UnionTree(P **prints, int64_t leafStart, int64_t leafEnd, int nBits, int cardinality, int leafLimit) :
	mCardinality(cardinality),
	mLeafLimit(leafLimit),
	mNBits(nBits),
	mLeafStart(leafStart),
	mSize(leafEnd - leafStart),
	mLeaves(prints),
	mSplitter(prints, nBits)
{
	// initialize member fields and allocate memory
	mTreeSize = MAX(2 * mSize - 1, 1);		// max size of binary-tree with "size" leaves
	mUnion = new P*[mTreeSize];
	mIsLeaf = new char[mTreeSize];
	mLeftChild = new int64_t[mTreeSize];
	mRightChild = new int64_t[mTreeSize];

	mNodes = 0;

	// build tree
	buildNode(leafStart, leafEnd);
}

// destructor
// delete all used resources
template <class TT>
UnionTree<TT>::~UnionTree() {
	for (int64_t i = 0; i < mNodes; i++) {
		delete mUnion[i];
	}

	delete[] mUnion;
	delete[] mIsLeaf;
	delete[] mLeftChild;
	delete[] mRightChild;
}

// recursively build UnionTree nodes and sub-nodes
template <class TT>
void UnionTree<TT>::buildNode(int64_t leafStart, int64_t leafEnd) {
	int64_t nLeaves;		// size of sub-tree range in leaves
	int64_t middle;			// index for sub-tree splitting
	int64_t thisNode;		// index of current node

	nLeaves = leafEnd - leafStart;

	thisNode = mNodes;
	mNodes++;

	assert(thisNode < mTreeSize);

	// create union of all leaves

	mUnion[thisNode] = new P(mLeaves[leafStart]);

	for (int64_t i = leafStart + 1; i < leafEnd; i++) {
		mUnion[thisNode]->join(mLeaves[i]);
	}

	if ((nLeaves < mLeafLimit) || mLeaves[leafStart]->isEqual(mUnion[thisNode])) {
		// create leaf node
		mIsLeaf[thisNode] = 1;
		mLeftChild[thisNode] = leafStart;
		mRightChild[thisNode] = leafEnd;
	} else {
		// create inner node
		mIsLeaf[thisNode] = 0;

		// split range by best bit
		mSplitter.reset();

		// count bits
		for (int64_t leaf = leafStart; leaf < leafEnd; leaf++) {
			for (int i = 0; i < mNBits; i++) {
				if (mLeaves[leaf]->getBit(i)) {
					mSplitter.incrementCount(i);
				}
			}
		}

		// split
		middle = mSplitter.splitRangeHalf(leafStart, leafEnd);

		// build left sub-tree
		mLeftChild[thisNode] = mNodes;
		buildNode(leafStart, middle);

		// build right sub-tree
		mRightChild[thisNode] = mNodes;
		buildNode(middle, leafEnd);
	}
}

// searching
// traverse the tree and visit only those sub-trees that don't surely underrun the tanimoto filter
template <>
void UnionTree<CLKTanimoto>::internalSearch(QueryResult *result, CLKTanimoto *queryPrint, int64_t node, int AB, float minTanimoto) {
	int AiU = queryPrint->intersectCard(mUnion[node]);

	if (((float) AiU / (AB - AiU)) >= minTanimoto) {
		if (mIsLeaf[node]) {
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
			// analyse sub-trees
			internalSearch(result, queryPrint, mLeftChild[node], AB, minTanimoto);
			internalSearch(result, queryPrint, mRightChild[node], AB, minTanimoto);
		}
	}
}

// searching
// traverse the tree and visit only those sub-trees that don't surely overrun the hamming distance
template <>
void UnionTree<CLKHamming>::internalSearch(QueryResult *result, CLKHamming *queryPrint, int64_t node, int AB, int maxHamming) {
	int AiU = queryPrint->intersectCard(mUnion[node]);

	if ((AB - (2 * AiU)) <= maxHamming) {
		if (mIsLeaf[node]) {
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
			// analyse sub-trees
			internalSearch(result, queryPrint, mLeftChild[node], AB, maxHamming);
			internalSearch(result, queryPrint, mRightChild[node], AB, maxHamming);
		}
	}
}

// perform a search for <query> that has <cardinality> filtered by <minTanimotoSimilarity>
template <>
inline void UnionTree<CLKTanimoto>::search(QueryResult *result, CLKTanimoto *queryPrint, int cardinality, float minTanimotoSimilarity) {
	internalSearch(result, queryPrint, 0, cardinality + mCardinality, minTanimotoSimilarity);
}

// perform a search for <query> that has <cardinality> filtered by <maxHammingDistance>
template <>
inline void UnionTree<CLKHamming>::search(QueryResult *result, CLKHamming *queryPrint, int cardinality, int maxHammingDistance) {
	internalSearch(result, queryPrint, 0, cardinality + mCardinality, maxHammingDistance);
}
#endif
