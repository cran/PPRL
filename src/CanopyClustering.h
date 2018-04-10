// Canopy.h
//
// Copyright (c) 2013
// Prof. Dr. Rainer Schnell
// Universitaet Duisburg-Essen
// Campus Duisburg
// Institut fuer Soziologie
// Lotharstr. 65
// 47057 Duisburg
//
// This file is part of the R-Package "canopyClustering".
//
// "canopyClustering" is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// "canopyClustering" is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with "canopyClustering". If not, see <http://www.gnu.org/licenses/>.

#ifndef CANOPYCLUSTERING_H
#define CANOPYCLUSTERING_H

#include <math.h>
#include "CLK.h"
#include "QueryResult.h"
#include "InputSupport.h"

// class CanopyClustering reveives an array of instances of the class T.
// The array will be clustered into canopies by a given minimal similarity,
// loose threshold and tight threshold. The objects of the same
// clusters will be matched by the given minimal similarity.

template<class T>
class CanopyClustering {
private:

  long long mSizeA;		// size of object-array part A
  long long mSizeB;		// size of object-array part B

  T **mObjects;			// array of objects

public:

  // constructor
  inline CanopyClustering(T **objects, long long sizeA, long long sizeB) {
    mSizeA = sizeA;
    mSizeB = sizeB;
    mObjects = objects;
  }

  // destructor
  inline ~CanopyClustering() {
    for (int i = 0; i < (mSizeA + mSizeB); i++) {
      if (mObjects[i]) {
        delete mObjects[i];
      }
    }

    delete[] mObjects;
  }

  // perform a search for <minSimilarity> and <looseThreshold>, <tightThreshold> and add the result to <result>
  inline void search(QueryResult *result, double minSimilarity, double looseThreshold, double tightThreshold) {
    double sim;
    long long clusterColSize = 0;
    long long *clusterCollection = new long long[mSizeA + mSizeB];
    for (int center = 0; center < mSizeA; center++) {
      clusterColSize = 0;
      if (mObjects[center]->isInP()) {
        mObjects[center]->removeFromP();
        clusterCollection[clusterColSize] = center;
        clusterColSize++;

        for (int i = (center + 1); i < (mSizeA + mSizeB); i++) {
          if (mObjects[i]->isInP()) {
            // check estimated similarity
            sim = mObjects[center]->estimatedSimilarity(mObjects[i]);
            if (sim >= looseThreshold) {
              clusterCollection[clusterColSize] = i;
              clusterColSize++;
              if (sim >= tightThreshold) {
                mObjects[i]->removeFromP();
              }
            }
          }
        }

        for (int i = 0; i < clusterColSize; i++) {
          for (int j = (i + 1); j < clusterColSize; j++) {
            if (mObjects[clusterCollection[i]]->getOrigin() != mObjects[clusterCollection[j]]->getOrigin()) {
              match(result, mObjects[clusterCollection[i]], mObjects[clusterCollection[j]], minSimilarity);
            }
          }
        }
      }
    }
    delete[] clusterCollection;
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

  inline void matchXOR(QueryResult *result, CLKXOR *clkA ,CLKXOR *clkB, double minSimilarity) {
    double sim;

    sim = clkA->tanimotoXOR(clkB, clkA->getCardinality() + clkB->getCardinality());

    if (sim >= minSimilarity) {
      match(result, clkB, minSimilarity);
    }
  }
};

// load  into internal tanimoto fingerprint data structure
template <class T>
void matchTanimotoCC(QueryResult *queryResult, vector<string> dataA, vector<string> ID1, vector<string> dataB, vector<string> ID2, double minSimilarity, double looseThreshold, double tightThreshold) {
  T **clks; // array of CLKs
  CanopyClustering<T> *cc;
  long long sizePrintsA;
  long long sizePrintsB;
  //int nBits;
  int64_t mSize;		// size of CLK-array used by mBuckets
  // initialize Fingerprint data structure (cardinality-map)
  T::init();

  sizePrintsA = dataA.size();

  if (sizePrintsA == 0) {
    return;
  }

  sizePrintsB = dataB.size();

  if (sizePrintsB == 0) {
    return;
  }

  // create array of Fingerprints
  clks = new T*[sizePrintsA + sizePrintsB];

  //nBits = 0;
  InputSupport<T> inputSupport;
  mSize = (int64_t)dataA.size();
  int lenBitVector; // maximal size of CLKs
  dataA.insert( dataA.end(), dataB.begin(), dataB.end() );
  ID1.insert( ID1.end(), ID2.begin(), ID2.end() );
  inputSupport.parseAllCLKs(ID1, dataA, &clks, &mSize, &lenBitVector,1 );
  // build sn data structure
  cc = new CanopyClustering<T>(clks, sizePrintsA, sizePrintsB);
  cc->search(queryResult, minSimilarity, looseThreshold, tightThreshold);
  delete cc;
}
#endif
