/*
 * AS16.h
 *
 *  Created on: 19.12.2016
 *      Author: Rukasz
 */

#ifndef AS16_H_
#define AS16_H_

#include <iostream>
#include <stdio.h>
#include <cstring>
#include <memory>
#include <vector>
#include <random>
#include <algorithm>
#include <iterator>
#include <Rcpp.h>
#include "Standardisation.h"
#include "CLK.h"

using namespace std;
vector<CLK*> CreateArmknechtCLK(int nrow, vector<string> IDc, vector<string> CLKin ,string password);
float CompareArmknechtCLK(CLK* d1, CLK* d2, float t );
vector<string> CreateArmknechtc(vector<string> IDc, vector<string> CLKin ,string password);
vector<string> CompareArmknecht(string id1, string d1, string id2, string d2, float t ) ;
vector<vector<string>> CreateCompareArmknechtAll(vector<string> id1, vector<string> CLK1 , vector<string> id2, vector<string> CLK2, string password, float t );
vector<vector<string>> CreateCompareArmknechtAllCLK(vector<string> id1, vector<string> CLK1 , vector<string> id2, vector<string> CLK2, string password, float t );
size_t computeHW(string s);
vector<double> computeFeasure(vector<vector<string>> in, int s);
vector<double> computeFeasureO(vector<string> id1, vector<string> CLK1 , vector<string> id2, vector<string> CLK2, string password, float t );
float CompareArmknechtCLK4(CLK* d1, CLK* d2, float t );
void CreateArmknechtCLK4(CLK ** res,vector<string> IDc, vector<string> CLKin ,string password);
#endif /* AS16_H_ */
