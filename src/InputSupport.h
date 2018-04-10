// InputSupport.h
//
// Copyright (c) 2017
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

#ifndef INPUTSUPPORT_H
#define INPUTSUPPORT_H

#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <vector>
//#include "Grid1D.h"

// maximal line size = length of ascii representation of CLK
#define STRSIZE 4000
using namespace std;

// objects of template class FileSupport provide functionalities to
// read CLK files in csv like format

template<class T>
class InputSupport {
private:

  // check, if a character is considered to be a white-space or seperator
  inline int isWS(char c) {
    return ((c == '"') || (c == '\'') || (c == ',') || (c == ';') || (c == ' ') || (c == '\t'));
  }

  // check, if a character is considered to be end of line
  inline int isEOL(char c) {
    return ((c == 10) || (c == 13) || (c == 0));
  }

  int parseLine(FILE *in, char *str, int64_t *idx1, int64_t *end1, int64_t *idx2, int64_t *end2);

public:

  T* parseCLK(string IDc, string CLKin, int64_t line);
  T* parseCLK(string IDc, string CLKin, int64_t line, int origin);
  void parseAllCLKs(vector<string> IDc, vector<string> CLKin, T ***clks, int64_t *size, int *nBits);
  void parseAllCLKs(vector<string> IDc, vector<string> CLKin, T ***clks, int64_t *size, int *nBits, int origin);
};

// parse line from file, return number of parsed fields
template<class T>
int InputSupport<T>::parseLine(FILE *in, char *str, int64_t *idx1, int64_t *end1, int64_t *idx2, int64_t *end2) {
  // read line from file
  if (fgets(str, STRSIZE, in) == NULL) {
    return 0;
  }

  // parse line, find start of first field
  *idx1 = 0;
  while (isWS(str[*idx1]) && (*idx1 < STRSIZE - 1)) {
    (*idx1)++;
  }

  // find end of first field
  *end1 = *idx1;
  while (!isWS(str[*end1]) && (*end1 < STRSIZE - 1) && !isEOL(str[*end1])) {
    (*end1)++;
  }

  if (!isEOL(str[*end1])) {
    // find start of second field
    *idx2 = *end1;
    while (isWS(str[*idx2]) && (*idx2 < STRSIZE - 1)) {
      (*idx2)++;
    }

    // find end of second field
    *end2 = *idx2;
    while (!isWS(str[*end2]) && (*end2 < STRSIZE - 1) && !isEOL(str[*end2])) {
      (*end2)++;
    }
  } else {
    *idx2 = 0;
    *end2 = 0;
  }

  // cut field 1
  str[*end1] = 0;

  if (*idx2 != *end2) {
    // if there are two fields, cut field 2
    str[*end2] = 0;
    return 2;
  }

  return 1;
}

// read a CLK from file
template<class T>
T* InputSupport<T>::parseCLK(string IDc, string CLKin, int64_t line) {
  char *idStr = new char[13];
  int lenCLK = CLKin.size(); //length of the CLK
  char *CLKstr = new char[lenCLK + 1];
  strcpy(CLKstr, CLKin.c_str());

  if (IDc.length() == 0 && CLKin.length() > 0) {
    // if Id is missing, use line as id

    sprintf(idStr, "%012" PRId64, line + 1);
    // create CLK
    return new T(idStr, CLKstr);
  } else if (IDc.length() > 0 && CLKin.length() > 0) {
    strcpy(idStr, IDc.c_str());
    // create CLK
    return new T(idStr, CLKstr);
  }
  delete[] idStr;
  delete[] CLKstr;
  return NULL;
}

// read a CLK from file
template<class T>
T* InputSupport<T>::parseCLK(string IDc, string CLKin, int64_t line, int origin) {
  char *idStr = new char[13];
  int lenCLK = CLKin.size(); //length of the CLK
  char *CLKstr = new char[lenCLK + 1];
  strcpy(CLKstr, CLKin.c_str());

  if (IDc.length() == 0 && CLKin.length() > 0) {
    // if Id is missing, use line as id

    sprintf(idStr, "%012" PRId64, line + 1);
    // create CLK
    return new T(idStr, CLKstr, origin);
  } else if (IDc.length() > 0 && CLKin.length() > 0) {
    strcpy(idStr, IDc.c_str());
    // create CLK
    return new T(idStr, CLKstr, origin);
  }
  delete[] idStr;
  delete[] CLKstr;
  return NULL;
}

// read CLK array from file
template<class T>
void InputSupport<T>::parseAllCLKs(vector<string> IDc, vector<string> CLKin, T ***clks, int64_t *size, int *nBits) {
  int64_t sizeCLKs;
  int maxLen;
  int len;
  T **readCLKs;

  sizeCLKs = CLKin.size(); //number of CLKs
  // second pass: count number of CLKs in file
  maxLen = 0;

  if (sizeCLKs == 0) {
    *size = 0;
    *nBits = 0;
    return;
  }

  // create array of CLKs
  readCLKs = new T*[sizeCLKs];

  // read CLKs from file
  for (int64_t i = 0; i < sizeCLKs; i++) {
    // parse line from file
    readCLKs[i] = parseCLK(IDc[i], CLKin[i], i);

    if (readCLKs[i] == NULL) {
      sizeCLKs = i;
      break;
    }

    // compute maximal length of CLKs
    len = readCLKs[i]->getLength();
    if (len > maxLen) {
      maxLen = len;
    }
  }

  *nBits = maxLen;
  *size = sizeCLKs;
  *clks = readCLKs;
}

// read CLK array from file
template<class T>
void InputSupport<T>::parseAllCLKs(vector<string> IDc, vector<string> CLKin, T ***clks, int64_t *size, int *nBits, int origin) {
  int64_t sizeCLKs;
  int maxLen;
  int len;
  int64_t sizeA = *size;
  T **readCLKs;

  sizeCLKs = CLKin.size(); //number of CLKs
  // second pass: count number of CLKs in file
  maxLen = 0;

  if (sizeCLKs == 0) {
    *size = 0;
    *nBits = 0;
    return;
  }

  // create array of CLKs
  readCLKs = new T*[sizeCLKs];

  // read CLKs from file
  for (int64_t i = 0; i < sizeA; i++) {
    // parse line from file
    readCLKs[i] = parseCLK(IDc[i], CLKin[i], i, 1);

    if (readCLKs[i] == NULL) {
      sizeCLKs = i;
      break;
    }

    // compute maximal length of CLKs
    len = readCLKs[i]->getLength();
    if (len > maxLen) {
      maxLen = len;
    }
  }

  // read CLKs from file
  for (int64_t i = sizeA; i < sizeCLKs; i++) {
    // parse line from file
    readCLKs[i] = parseCLK(IDc[i], CLKin[i], i, 2);

    if (readCLKs[i] == NULL) {
      sizeCLKs = i;
      break;
    }

    // compute maximal length of CLKs
    len = readCLKs[i]->getLength();
    if (len > maxLen) {
      maxLen = len;
    }
  }

  *nBits = maxLen;
  *size = sizeCLKs;
  *clks = readCLKs;
}
#endif
