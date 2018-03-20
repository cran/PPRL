// Enable C++11 via this plugin
// [[Rcpp::plugins("cpp11")]]


#include "BloomFilterLinkagerWrapper.h"

using namespace Rcpp;


//[[Rcpp::export(".BloomfilterLinkagec")]]
DataFrame BloomfilterLinkagec(SEXP IDA_, SEXP dataA_, SEXP blockingdataA_,
                              SEXP IDB_, SEXP dataB_, SEXP blockingdataB_, SEXP method_,
                              SEXP blocking_, SEXP threshold_, SEXP windowSize_,
                              SEXP looseThreshold_, SEXP tightThreshold_, SEXP threads_, SEXP leaflimit_, SEXP symdex_) {
  string method = Rcpp::as < string > (method_);
  string blocking = Rcpp::as < string > (blocking_);
  float threshold = Rcpp::as<float>(threshold_);
  int windowSize = Rcpp::as<int>(windowSize_);
  int threads = Rcpp::as<int>(threads_);
  int leaflimit = Rcpp::as<int>(leaflimit_);
  int symdex = Rcpp::as<int>(symdex_);
  float looseThreshold = Rcpp::as<float>(looseThreshold_);
  float tightThreshold = Rcpp::as<float>(tightThreshold_);
  //  // Handle IDs, Ids can either be of type int or character
  vector<string> ID1, ID2;
  vector<int> ID1Int, ID2Int;
  int ID1size, ID2size;

  if (TYPEOF(IDA_) == STRSXP) { // case vector of ID1 is of type character
    ID1 = Rcpp::as < std::vector<string> > (IDA_);
    ID1size = ID1.size();
  } else if (TYPEOF(IDA_) == INTSXP) { // case vector of ID1 is of type int
    ID1Int = Rcpp::as < std::vector<int> > (IDA_);
    ID1size = ID1Int.size();
     Rcpp::Rcout << "Info: IDA is of type int. It will be transformed to character."
                << std::endl;
    if (ID1size > 0) {
      for (int j = 0; j < ID1size; j++) {
        ID1.push_back(to_string(ID1Int[j])); //Copy the vector to the string
      }
    }
  } else {
    Rcpp::Rcerr << "Input IDA must be a vector of Type character or int.\n";
    return 0;
  }
  if (TYPEOF(IDB_) == STRSXP) { // case vector of ID2 is of type character
    ID2 = Rcpp::as < std::vector<string> > (IDB_);
    ID2size = ID2.size();
  } else if (TYPEOF(IDB_) == INTSXP) { // case vector of ID2 is of type int
    ID2Int = Rcpp::as < std::vector<int> > (IDB_);
    ID2size = ID2Int.size();
     Rcpp::Rcout << "Info: IDB is of type int. It will be transformed to character."
         << endl;
    if (ID2size > 0) {
      for (int j = 0; j < ID2size; j++) {
        ID2.push_back(to_string(ID2Int[j])); //Copy the vector to the string
      }
    }
  } else {
    Rcpp::Rcerr << "Input IDB must be a vector of Type character or int.\n";
    return 0;
  }
  //Handle variables, vars can either be of type int, float or character
  vector < string > varsV1 = prepareData(dataA_, "Linking vars1", false);
  vector < string > varsV2 = prepareData(dataB_, "Linking vars2", false);

  // Handle blocking variables,  blocking vars can either be of type int, float or character
  vector<string> blockingvarsV1, blockingvarsV2;
  if (!Rf_isNull(blockingdataA_)) {
    blockingvarsV1 = prepareData(blockingdataA_, "Blocking vars1", false);
  } else {
    for (unsigned i = 0; i < varsV1.size(); i++)
      blockingvarsV1.push_back("0");
  }

  if (!Rf_isNull(blockingdataB_)) {
    blockingvarsV2 = prepareData(blockingdataB_, "Blocking vars2", false);
  } else {
    for (unsigned i = 0; i < varsV2.size(); i++)
      blockingvarsV2.push_back("0");
  }

  //check sizes
  if (varsV1.size() % ID1size != 0) {
    Rcpp::Rcerr << " ID-Vector 1 and Input-Vector 1 must have the same size. "
         << varsV1.size() << " " << ID1size << endl;
    return 0;
  }
  if (varsV2.size() % ID2size != 0) {
    Rcpp::Rcerr << " ID-Vector 2 and Input-Vector 2 must have the same size. "
         << endl;
    return 0;
  }
  //set StringVectorData
  MTB_StringVectorData d1 = MTB_StringVectorData("var1", ID1, varsV1, blockingvarsV1);
  MTB_StringVectorData d2 = MTB_StringVectorData("var2", ID2, varsV2, blockingvarsV2);
  //set merge configuration

  MergingConfiguration mc;
  mc.setAlgorithm(method);
  mc.setThreshold(threshold);
  mc.setBlocking(blocking);
  mc.setWindowSize(windowSize);
  mc.setLooseThreshold(looseThreshold);
  mc.setTightThreshold(tightThreshold);
  mc.setThreads(threads);
  mc.setLeaflimit(leaflimit);
  mc.setPreproc(symdex);

  MTB_Result res;
  if (find(MBTmethods.begin(), MBTmethods.end(), method) != MBTmethods.end()){
    res = mbtWrapper(d1, d2, mc);
  } else if (method.compare("CCtan") == 0 || method.compare("CCtanXOR") == 0){
    res = canopyClusteringWrapper(d1, d2, mc);
  } else if (method.compare("SNtan") == 0 || method.compare("SNtanXOR") == 0){
    res = sortedNeighbourhoodWrapper(d1, d2, mc);
  }
  string name = mc.getAlgorithmName();
  string nameSub = mc.getAlgorithmName().substr(1, 3); //Name of the method used
  if (nameSub == "ham")
    name = "hamming";
  if (nameSub == "tan"|| name == "CCtan" || name == "CCtanXOR" || name == "SNtan" || name == "SNtanXOR"){
    name = "tanimoto";
    }
  return DataFrame::create(Named("ID1") = wrap(res.getID1()),
                          Named("ID2") = wrap(res.getID2()),
                          Named(name) = wrap(res.getRes()),
                          Named("stringsAsFactors") = false);


}

MTB_Result mbtWrapper(MTB_StringVectorData d1, MTB_StringVectorData d2,
                      MergingConfiguration mc) {
  MTB_Result res;
  int threads = mc.getThreads(); //
  int64_t size = 0; //
  int leafLimit = mc.getLeaflimit(); //
  int preproc = mc.getPreproc(); //
  float minSimilarity = mc.getThreshold(); //

  if ((minSimilarity <= 0.0) || (minSimilarity > 1.0)) {
    Rcpp::Rcerr << "Error: min-similarity must be greater than 0.0 and less than or equal 1.0\n";
    return res;
  }

  if (threads < 1) {
    Rcpp::Rcerr <<
            "Error: the number of worker-threads must be at least 1!\n";
    return res;
  }
  vector<MergingConfiguration> merging;
  merging.push_back(mc);
  vector<MTB_MergeData> md = initMergeData(d1, d2, merging); // blocking takes place
  MTB_StringVectorData m1, m2;
  for (MTB_MergeData m : md) {
    QueryResult queryResult(0,0);
    m1 = m.getData1();
    m2 = m.getData2();

  // initialize CLK data structure (cardinality-map)
  CLK::init();
    if (mc.getAlgorithmName() == "mtan") {
      GridMbtTan *grid = loadTree<GridMbtTan>(m1.getID(), m1.getData(), size,
                                            threads, leafLimit);
      searchTree<GridMbtTan>(&queryResult, grid, m2.getID(), m2.getData(),
                           preproc, minSimilarity,
                           threads);
    } else if (mc.getAlgorithmName() == "mham") {
      GridMbtHam *grid = loadTree<GridMbtHam>(m1.getID(), m1.getData(), size,
                                            threads, leafLimit);
      searchTree<GridMbtHam>(&queryResult, grid, m2.getID(), m2.getData(),
                           preproc, (1.0 - minSimilarity) * grid->getNBits(),
                           threads);
    } else if (mc.getAlgorithmName() == "utan") {
      GridUnionTan *grid = loadTree<GridUnionTan>(m1.getID(), m1.getData(),
                                                size, threads, leafLimit);
      searchTree<GridUnionTan>(&queryResult, grid, m2.getID(), m2.getData(),
                             preproc, minSimilarity,
                             threads);
    } else if (mc.getAlgorithmName() == "uham") {
      GridUnionHam *grid = loadTree<GridUnionHam>(m1.getID(), m1.getData(),
                                                size, threads, leafLimit);
      searchTree<GridUnionHam>(&queryResult, grid, m2.getID(), m2.getData(),
                             preproc, (1.0 - minSimilarity) * grid->getNBits(),
                             threads);
    } else {
      Rcpp::Rcerr << "Error: Unknown algorithm! " << mc.getAlgorithmName()
         << endl;
    return res;
    }

    QueryResultNodeStruct *node = queryResult.getRootNode();

  //get result
    if (node != NULL) {
      res.addResult(node->mPrint->getId(), node->mQueryId, node->mSimilarity);
      while (node->mRight != NULL) {
        node = node->mRight;
        res.addResult(node->mPrint->getId(), node->mQueryId,
                    node->mSimilarity);
      }
    }
  }
  return res;
}

MTB_Result canopyClusteringWrapper(MTB_StringVectorData d1, MTB_StringVectorData d2, MergingConfiguration mc){
  MTB_Result res;
  vector<string> queriesID;
  vector<string> matchesID;
  vector<double> similarities;
  //long long idx;
  vector<MergingConfiguration> merging;
  merging.push_back(mc);
  vector<MTB_MergeData> md = initMergeData(d1, d2, merging); // blocking takes place
  MTB_StringVectorData m1, m2;
  for (MTB_MergeData m : md) {
    QueryResult queryResult(0,0);
    m1 = m.getData1();
    m2 = m.getData2();
    if(mc.getAlgorithmName().compare("CCtan") == 0) {
    // method "tanimoto"

      matchTanimotoCC<CLK>(&queryResult, m1.getData(), m1.getID(), m2.getData(), m2.getID(), mc.getThreshold(), mc.getLooseThreshold(), mc.getTightThreshold());

    } else  if(mc.getAlgorithmName().compare("CCtanXOR") == 0) {
    // method "tanimotoXOR"

     matchTanimotoCC<CLKXOR>(&queryResult, m1.getData(), m1.getID(), m2.getData(), m2.getID(), mc.getThreshold(), mc.getLooseThreshold(), mc.getTightThreshold());
    }

    QueryResultNodeStruct *node = queryResult.getRootNode();

  //get result
      if (node != NULL) {
      res.addResult(node->mQueryId, node->mMatchId, node->mSimilarity);
      while (node->mRight != NULL) {
        node = node->mRight;
        res.addResult(node->mQueryId, node->mMatchId, node->mSimilarity);
      }
    }
  }
  return res;
}

// searching for similar clks
//DataFrame sortedNeighbourhood(SEXP IDA, SEXP blockingdataA, SEXP IDB, SEXP blockingdataB, double minSimilarity, int windowSize, SEXP method) {
MTB_Result sortedNeighbourhoodWrapper(MTB_StringVectorData d1, MTB_StringVectorData d2, MergingConfiguration mc){
  MTB_Result res;
  //SEXP result;
  //SEXP names;
  //SEXP queries;
  //SEXP matches;
  //SEXP similarities;
  //double *similaritiesPtr;
//long long idx;
  //long long sizeResult;

  vector<MergingConfiguration> merging;
  merging.push_back(mc);
  vector<MTB_MergeData> md = initMergeData(d1, d2, merging); // blocking takes place
  MTB_StringVectorData m1, m2;
  for (MTB_MergeData m : md) {
    QueryResult queryResult(0,0);
    m1 = m.getData1();
    m2 = m.getData2();
    if(mc.getAlgorithmName().compare("SNtan") == 0) {
      matchTanimotoSN<CLK>(&queryResult, m1.getData(), m1.getID(), m2.getData(), m2.getID(), mc.getThreshold(), mc.getWindowSize(), mc.getAlgorithmName());
    } else  if(mc.getAlgorithmName().compare("SNtanXOR") == 0) {
      matchTanimotoSN<CLKXOR>(&queryResult, m1.getData(), m1.getID(), m2.getData(), m2.getID(), mc.getThreshold(), mc.getWindowSize(), mc.getAlgorithmName());
    }

  QueryResultNodeStruct *node = queryResult.getRootNode();
  //get result

    if (node != NULL) {
      res.addResult(node->mQueryId, node->mMatchId, node->mSimilarity);
      while (node->mRight != NULL) {
        node = node->mRight;
        res.addResult(node->mQueryId, node->mMatchId, node->mSimilarity);
      }
    }
  }
  return res;
}
