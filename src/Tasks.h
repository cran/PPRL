// Tasks.h
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

#ifndef TASKS_H
#define TASKS_H

// instances of template class TaskCreateTree hold the parameters
// for performing the creation of a search tree
template <class T>
class TaskCreateTree {
	private:

        T **mTree;			// address where to store the new search tree pointer
        typename T::P **mPrints;	// pointer to array of CLKs
        int64_t mLeafStart;		// cluster starting position in prints
        int64_t mLeafEnd;		// end of cluster
        int mNBits;			// maximal size of CLK in bits
        int mCardinality;		// cluster cardinality
        int mLeafLimit;			// leaf limit for SearchTree creation

	public:

	void set(T **tree, typename T::P **prints, int64_t leafStart, int64_t leafEnd, int nBits, int cardinality, int leafLimit) {
		mTree = tree;
		mPrints = prints;
		mLeafStart = leafStart;
		mLeafEnd = leafEnd;
		mNBits = nBits;
		mCardinality = cardinality;
		mLeafLimit = leafLimit;
	}

	void run() {
		*mTree = new T(mPrints, mLeafStart, mLeafEnd, mNBits, mCardinality, mLeafLimit);
	}
};

// instances of template class TaskSearchTreeRange hold the parameters
// for searching in an array of search trees
template <class T>
class TaskSearchTreeRange {
	private:

        T **mTrees;			// pointer to the SearchTree array to search
        int mMin;			// start of the range in the array
        int mMax;			// end of the range in the array
        QueryResult *mResult;		// QueryResult for storing the results
        typename T::P *mQuery;		// query CLK to search for
        int mCardinality;		// cardinality of query
        typename T::S mFilter;		// filter criteria

	public:

	void set(T **trees, int min, int max, QueryResult *result, typename T::P *query, int cardinality, typename T::S filter) {
		mTrees = trees;
		mMin = min;
		mMax = max;
		mResult = result;
		mQuery = query;
		mCardinality = cardinality;
		mFilter = filter;
	}

	void run() {
		for (int i = mMin; i < mMax; i++) {
			if (mTrees[i]) {
				mTrees[i]->search(mResult, mQuery, mCardinality, mFilter);
			}
		}
	}
};

// instances of template class TaskSearchTreeSymdex hold the parameters
// for searching a Symdex query cluster in an array of search trees.
template <class T>
class TaskSearchTreeSymdex {
	private:

        T **mTrees;			// pointer to the SearchTree array to search
        int mMin;			// start of the range in the array
        int mMax;			// end of the range in the array
        QueryResult *mResult;		// QueryResult for storing the results
        typename T::P **mQueryPrints;	// array of query CLKs to search for
        int64_t mQStart;		// start of cluster in array
        int64_t mQEnd;			// end of cluster in array
        int mCardinality;		// common cardinality of query prints
        typename T::S mFilter;		// filter criteria

	public:

	void set(T **trees, int min, int max, QueryResult *result, typename T::P **queryPrints, int64_t qStart, int64_t qEnd, int cardinality, typename T::S filter) {
		mTrees = trees;
		mMin = min;
		mMax = max;
		mResult = result;
		mQueryPrints = queryPrints;
		mQStart = qStart;
		mQEnd = qEnd;
		mCardinality = cardinality;
		mFilter = filter;
	}

	void run() {
		for (int i = mMin; i < mMax; i++) {
			if (mTrees[i]) {
				for (int64_t q = mQStart; q < mQEnd; q++) {
					mTrees[i]->search(mResult, mQueryPrints[q], mCardinality, mFilter);
                                }
                        }
		}
	}
};
#endif
