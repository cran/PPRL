// SortedNeighbourhood.h
//
// Copyright (c) 2013
// Prof. Dr. Rainer Schnell
// Universitaet Duisburg-Essen
// Campus Duisburg
// Institut fuer Soziologie
// Lotharstr. 65
// 47057 Duisburg
//
// This file is part of the R-Package "sortedNeighbourhood".
//
// "sortedNeighbourhood" is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// "sortedNeighbourhood" is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with "sortedNeighbourhood". If not, see <http://www.gnu.org/licenses/>.

#ifndef SORTEDNEIGHBOURHOOD_H
#define SORTEDNEIGHBOURHOOD_H

#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include "Misc.h"
#include <math.h>
#include "CLK.h"
#include "QueryResult.h"
#include <string.h>
#include "InputSupport.h"

using namespace std;
// The class SortedNeighbourhood receives an array of instances of the class T.
// The array will be sorted according to the sortkey of the objects.
// A sorted neighbourhood matching will be performed by a given minimal similarity
// and a window size.

template <class T>
class SortedNeighbourhood {
private:

  long long mSize;		// size of object-array
  T **mObjects;			// array of objects

public:

  // constructor
  // objects      : pointer on object array
  // size         : size of object array
  inline SortedNeighbourhood(T **objects, long long size) {
    mSize = size;
    mObjects = objects;

    // sort prints by sortkey
    sortObjects(0, size-1);
  }

  // destructor
  inline ~SortedNeighbourhood() {
    for (int i = 0; i < mSize; i++) {
      if (mObjects[i]) {
        delete mObjects[i];
      }
    }

    delete[] mObjects;
  }

  // perform a search for <minSimilarity> and <windowSize> and add the result to <result>
  inline void search(QueryResult *result, double minSimilarity, int windowSize, string method) {
    int win_end;

    for (int i = 0; i < (mSize - 1); i++) {
      win_end = MIN(i + windowSize, mSize);

      // compare first item with all others in the window

      T *firstObject = mObjects[i];

      for (int j = (i + 1); j < win_end; j++) {
        if (firstObject->getOrigin() != mObjects[j]->getOrigin()) {
          match(result, firstObject, mObjects[j], minSimilarity);

        }
      }
    }
  }

  // generic similarity estimation

  inline double estimatedSimilarity(QueryResult *result, T *clkA ,T *clkB) {
    return tanimotoXOR(clkB, clkA->getSortKey() + clkB->getSortKey());
  }

  // generic similarity check

  inline void match(QueryResult *result, T *clkA ,T *clkB, double minSimilarity) {
    double sim = clkA->tanimoto(clkB);
    if (sim >= minSimilarity) {
      if (clkA->getOrigin() == 1) {
        result->add(clkA->getId(), clkB->getId(), sim);
      } else {
        result->add(clkB->getId(), clkA->getId(), sim);
      }
    }
  }

  // generic similarity check

  inline void matchXOR(QueryResult *result, T *clkA ,T *clkB, double minSimilarity) {
    double sim;

    sim = clkA->tanimotoXOR(clkB, clkA->getSortKey() + clkB->getSortKey());

    if (sim >= minSimilarity) {
      double sim = clkA->tanimoto(clkB);
      if (sim >= minSimilarity) {
        if (clkA->getOrigin() == 1) {
          result->add(clkA->getId(), clkB->getId(), sim);
        } else {
          result->add(clkB->getId(), clkA->getId(), sim);
        }
      }
    }
  }

private:

  void sortObjects(long long lowerIdx, long long upperIdx);
};

// quick sort objects bei sortkey
template<class T>
void SortedNeighbourhood<T>::sortObjects(long long lowerIdx, long long upperIdx) {
  if (lowerIdx < upperIdx) {
    T *swap;
    long long l = lowerIdx;
    int pivot = mObjects[upperIdx]->getCardinality();

    for (long long u = lowerIdx; u < upperIdx; u++) {
      if (mObjects[u]->getCardinality() <= pivot) {
        swap = mObjects[l];
        mObjects[l] = mObjects[u];
        mObjects[u] = swap;
        l++;
      }
    }

    swap = mObjects[l];
    mObjects[l] = mObjects[upperIdx];
    mObjects[upperIdx] = swap;

    sortObjects(lowerIdx, l - 1);
    sortObjects(l + 1, upperIdx);
  }
}


// load files into internal tanimoto fingerprint data structure
template <class T>
void matchTanimotoSN(QueryResult *queryResult, vector<string> blockingdataA_, vector<string> ID1, vector<string> blockingdataB_, vector<string> ID2, double minSimilarity, int windowSize, string method) {
  T **clks;
  SortedNeighbourhood<T> *sn;
  long long sizePrintsA;
  long long sizePrintsB;
  int64_t mSize;		// size of CLK-array used by mBuckets

  // initialize Fingerprint data structure (cardinality-map)
  T::init();

  sizePrintsA = blockingdataA_.size();

  if (sizePrintsA == 0) {
    return;
  }

  sizePrintsB = blockingdataB_.size();

  if (sizePrintsB == 0) {
    return;
  }

  // create array of Fingerprints
  clks = new T*[sizePrintsA + sizePrintsB];



  InputSupport<T> inputSupport;
  mSize = (int64_t)blockingdataA_.size();
  int lenBitVector; // maximal size of CLKs
  blockingdataA_.insert( blockingdataA_.end(), blockingdataB_.begin(), blockingdataB_.end() );
  ID1.insert( ID1.end(), ID2.begin(), ID2.end() );
  inputSupport.parseAllCLKs(ID1, blockingdataA_,  &clks, &mSize, &lenBitVector,1 );

  // build sn data structure
  sn = new SortedNeighbourhood<T>(clks, sizePrintsA + sizePrintsB);
  sn->search(queryResult, minSimilarity, windowSize, method);
  delete sn;

}

#endif
