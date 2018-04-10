// Grid1D.h
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

#ifndef GRID1D_H
#define GRID1D_H

#include <stdio.h>
#include <iostream>
#include "CLK.h"
#include "CLKSorter.h"
//#include "MultibitTree.h"
#include "ThreadPool.h"
#include "Tasks.h"
#include "InputSupport.h"

using namespace std;

// Objects of template class Grid1D hold an array of instances of the template
// classes MultibitTree or UnionTree.
// In each tree all CLKs of the same cardinality are stored.
// Knowing the queries cardinality and the similarity threshold a search can
// be reduced on a relevant range of trees.
// The Grid1D also uses the template class ThreadPool for concurrently work
// on different different trees.
template<class TT> // damit bleibt offen welchen Typ die Klasse Grid1D hat
class Grid1D {
public:

	typedef TT T;
	typedef typename TT::P P; //typename deutet an, dass TT::P der Name eines Typs ist.
	typedef typename TT::S S;

private:

	P **mPrints;		// array of CLKs
	T **mBuckets;		// array of search trees
	int mNBits;		// maximal size of CLKs
	int64_t mSize;		// size of CLK-array used by mBuckets

public:

	// constructor:
	//
	// read CLKs from file and sort them by cardinality and
	// create a search tree for each cardinality
	//
	// prints       : pointer on CLK array
	// size         : size of <prints>
	// threads      : number of parallel threads passed to ThreadPool
	// leafLimit    : leaf limit parameter passed to all MultibitTrees
	Grid1D(vector<string> IDc, vector<string> CLKin, int64_t size, int threads, int leafLimit) {
		CLKSorter<P> fpSorter;
		InputSupport<P> inputSupport;
		TaskCreateTree<T> task;

		//parse intput to clk
		mSize = size;
		inputSupport.parseAllCLKs(IDc, CLKin, &mPrints, &mSize, &mNBits);

		ThreadPool<TaskCreateTree<T> > *workerPool = new ThreadPool<TaskCreateTree<T> >(threads);

		mBuckets = new T*[mNBits + 1];

		// sort prints into clusters by cardinality
		fpSorter.cluster(mPrints, mSize, mNBits);

		// create a MultibitTree for each cardinality cluster
		for (int i = 0; i < (mNBits + 1); i++) {
			if (fpSorter.getSize(i) > 0) {
				task.set(&mBuckets[i], mPrints, fpSorter.getStart(i), fpSorter.getEnd(i), mNBits, i, leafLimit);
				workerPool->dispatchTask(task);
			} else {
				mBuckets[i] = NULL;
			}
		}

		// wait for running threads
		workerPool->wait();

		delete workerPool;
	}

	// delete grid data
	~Grid1D() {
		for (int i = 0; i < mNBits; i++) {
			if (mBuckets[i]) {
				delete mBuckets[i];
			}
		}

		delete[] mBuckets;
	}

	// get size of print array
	inline int64_t getSize() {
		return mSize;
	}

	// get maximal print size
	inline int getNBits() {
		return mNBits;
	}

	// search query prints from input file in tree
	void searchFile(vector<string> IDc, vector<string> CLKin, S filter, QueryResult *queryResult, int threads) {
		P *queryPrint;
		unsigned i;
		int min, max, card;
		InputSupport<P> inputSupport;
		TaskSearchTreeRange<T> task;
		ThreadPool<TaskSearchTreeRange<T> > *workerPool = new ThreadPool<TaskSearchTreeRange<T> >(threads);

		for (i = 0; i < CLKin.size(); i++) {
			// for each line parse CLK
			queryPrint = inputSupport.parseCLK(IDc[i], CLKin[i], i);


			// search only in buckets with suitable cardinality
			card = queryPrint->cardinality();
			min = MAX(P::lowerBound(filter, card), 0);
			max = MIN(P::upperBound(filter, card), mNBits + 1);

			// search print in grid
			task.set(mBuckets, min, max, queryResult, queryPrint, card, filter);
			workerPool->dispatchTask(task);
		}
		queryResult->setSizeLastSearch(i);

		// wait for running threads
		workerPool->wait();

		delete workerPool;
	}

	// search query prints from input file in tree with symdex pre-processing
	void searchFileSymdex(vector<string> IDc, vector<string> CLKin, S filter, QueryResult *queryResult, int threads) {
		CLKSorter<P> fpSorter;
		P **queryPrints;
		int64_t sizePrints = 0;
		int nBits;
		int min, max;
		InputSupport<P> inputSupport;
		TaskSearchTreeSymdex<T> task;
		ThreadPool<TaskSearchTreeSymdex<T> > *workerPool = new ThreadPool<TaskSearchTreeSymdex<T> >(threads);

		// read query prints from file
		inputSupport.parseAllCLKs(IDc, CLKin, &queryPrints, &sizePrints, &nBits);

		// sort prints into clusters by cardinality
		fpSorter.cluster(queryPrints, sizePrints, nBits);

		// for each cluster call asynchonous search-method
		for (int i = 0; i < (mNBits + 1); i++) {
			if (fpSorter.getSize(i) > 0) {
				// search only in buckets with suitable cardinality
				min = MAX(P::lowerBound(filter, i), 0);
				max = MIN(P::upperBound(filter, i), mNBits + 1);

				task.set(mBuckets, min, max, queryResult, queryPrints, fpSorter.getStart(i), fpSorter.getEnd(i), i,
						filter);
				workerPool->dispatchTask(task);
			}
		}

		queryResult->setSizeLastSearch(sizePrints);

		// wait for running threads
		workerPool->wait();

		delete workerPool;
	}
};
#endif
