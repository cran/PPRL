#include <Rcpp.h>
#include "AS16.h"
#include "CLK.h"

using namespace Rcpp;

/**
* Wrapper function for Create
*
* This method is not necessary for compare, it isjust to see the output of the method create
*
* @param id1 IDs of CLK1
* @param CLK1 vector of CLKs to be encoded
* @param id2 IDs of CLK2
* @param CLK2 vector of CLKs to be encoded
* @param password codeword to create vectors
* @param t Tanomito similartiy to be tested
*/
//[[Rcpp::export]]
CharacterVector CreateAS16(SEXP ID, CharacterVector data, SEXP password) {
  //convert input from R to cpp
  vector<string> CLKin =Rcpp::as<std::vector<string> > (data);
  vector<string> IDc =Rcpp::as<std::vector<string> > (ID);
  string password_ =  as<string>(password);
  //output
  CharacterVector CLKout;

  CLKout =CreateArmknechtc(IDc, CLKin, password_);
  return CLKout;
}


/**
* Wrapper function for Create and Compare
*
* Creates two vectors from CLKs according to Armknechts method create
* and compares them to each other
*
* @param d1_ encoded record 1
* @param h1 Hamming weights of the (unknown) record 1
* @param d2_ encoded record 2
* @param h2 Hamming weights of the (unknown) record 2
* @param password codeword to create vectors
* @param t Tanomito similartiy to be tested
*/
//[[Rcpp::export]]
DataFrame CompareAS16( CharacterVector IDA, CharacterVector dataA,  CharacterVector IDB, CharacterVector dataB, SEXP password, float t =0.85) {
  //convert input from R to cpp
  vector<string> d1 = Rcpp::as<std::vector<string> > (dataA);
  vector<string> d2 = Rcpp::as<std::vector<string> > (dataB);
  vector<string> id1 = Rcpp::as<std::vector<string> > (IDA);
  vector<string> id2 = Rcpp::as<std::vector<string> > (IDB);
  string password_ =  as<string>(password);
  DataFrame df;
  CharacterVector ID1, ID2;
  NumericVector value1, value2, value3, value4 ;
  float x;
  int nrow = d1.size();
  vector<CLK*> createA;
  createA = CreateArmknechtCLK(nrow, id1, d1, password_);
  nrow = d2.size();
  vector<CLK*> createB;
  createB = CreateArmknechtCLK(nrow, id2, d2, password_);
  for(unsigned i = 0; i< d1.size(); i++){
    for(unsigned j = 0; j< d2.size(); j++){
      x = CompareArmknechtCLK(createA[i], createB[j],  t);
      if (x<=((1-t)/(t+1))){
        ID1.push_back(createA[i]->getId());
        ID2.push_back(createB[j]->getId());
        value1.push_back(x);
        value2.push_back(((1-t)/(t+1)));
        value3.push_back(x <= ((1-t)/(t+1)));
        value4.push_back(((1-x)/(x+1)));
      }
    }
  }

  df=  DataFrame::create(Named("ID1") = ID1 ,
                         Named("ID2") = ID2,
                         //Named("ctr/h1+h2") = value1,
                         // Named("1-t/t+1") = value2,
                         Named("(1-x)/(x+1)") = value4,
                         // Named("Res") = (value3 == 1),

                         Named("stringsAsFactors") = false);
  return df;
}
