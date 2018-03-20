/*
 * MBT.h
 *
 *  Created on: 29 Sep 2017
 *      Author: rukasz
 */

#ifndef MBT_H_
#define MBT_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <vector>
#include "CLK.h"
#include "MultibitTree.h"
#include "UnionTree.h"
#include "Grid1D.h"
#include "MTB_Result.h"
#include "MTB_MergeData.h"

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

// types of required algorithms
typedef Grid1D<MultibitTree<CLKTanimoto> > GridMbtTan;
typedef Grid1D<MultibitTree<CLKHamming> > GridMbtHam;
typedef Grid1D<UnionTree<CLKTanimoto> > GridUnionTan;
typedef Grid1D<UnionTree<CLKHamming> > GridUnionHam;

// template function to load a search tree for a given algorithm
template<class T>
T* loadTree(vector<string> IDc, vector<string> CLKin, int64_t size, int threads,
		int leafLimit) {

  	T *grid;

// load tree
	grid = new T(IDc, CLKin, size, threads, leafLimit);
	size = grid->getSize();

	if (size <= 0) {
	  Rcpp::Rcerr << "Error: No data found while trying to load tree data!" << endl;
	  return grid;
	}

	return grid;
}

// template function to start search with a given algorithm
template<class T>
void searchTree(QueryResult *queryResult, T *grid, vector<string> IDc, vector<string> CLKin,
	int preproc, float minSimilarity, int threads) {

	// search tree for query prints
	if (preproc) {
		grid->searchFileSymdex(IDc, CLKin, minSimilarity, queryResult,
				threads);
	} else {

		grid->searchFile(IDc, CLKin, minSimilarity, queryResult, threads);
	}

	delete grid;
}

#endif /* MBT_H_ */
