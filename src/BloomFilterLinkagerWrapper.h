#ifndef BLOOMFILTERLINKAGEWRAPPER_H
#define BLOOMFILTERLINKAGEWRAPPER_H

#define __STDC_FORMAT_MACROS
#include <Rcpp.h>
#include <string>
#include <inttypes.h>
#include "MBT.h"
#include "MTB_MergeData.h"
#include "prepareData.h"
#include "Misc.h"
#include "CanopyClustering.h"
#include "SortedNeighbourhood.h"
#include "InputSupport.h"

using namespace Rcpp;
using namespace std;

/**
 * methods used in multibit trees
 */
vector<string> MBTmethods = {"mtan", "utan", "mham", "uham"};

/**
 * wrapper for multibit trees
 */
MTB_Result mbtWrapper(MTB_StringVectorData d1, MTB_StringVectorData d2, MergingConfiguration mc);


/**
 * wrapper for canopy clustering
 */
MTB_Result canopyClusteringWrapper(MTB_StringVectorData d1, MTB_StringVectorData d2, MergingConfiguration mc);

/**
 * wrapper for sorted Neighbourhood
 */
MTB_Result sortedNeighbourhoodWrapper(MTB_StringVectorData d1, MTB_StringVectorData d2, MergingConfiguration mc);


#endif
