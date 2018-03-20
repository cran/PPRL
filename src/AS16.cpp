#include "AS16.h"
using namespace std;

/**
* Wrapper function for Create
*/

vector<string> CreateArmknechtc(vector<string> IDc, vector<string> CLKin ,string password) {
  //output
  vector<string> CLKout;
  // random bit
  bool b;
  // compute length of Bloom Filter
  int lenBloom;
  if (CLKin.size()>0)
    lenBloom = CLKin[0].size();
  else{
    Rcpp::Rcerr << "Missing input data.";
    return CLKout;
  }
  // Create CLK r and w
  CLK* r = new CLK(lenBloom);
  r->init();
  CLK *w = new CLK(lenBloom);
  w->init();
  char *str = new char[lenBloom + 1];
  char id[100];

  //Generate lenBloom/2 randoms bit b dependened on password as seed
  seed_seq seed(password.begin(), password.end());
  default_random_engine engine(seed);
  vector<int> randomBits(lenBloom/2);
  uniform_int_distribution<int> distr(0, lenBloom - 1);
  generate(begin(randomBits), end(randomBits), [&]() {return distr(engine);});

  //for all lines
  for (int i =0; i < (int)CLKin.size() ; i++){
    //fill CLK r with input
    r->clear();
    strcpy(id, IDc[i].c_str()); //
    strcpy(str, CLKin[i].c_str());
    r->copyFromString(id, str);
    // fill CLK w
    w->clear();
    for (int i =1; i < lenBloom/2 ; i++){
      //Choose a random bit b
      b=randomBits[i]%2;
      if (b==1){
        w->setBit(2*i-1);
        w->setBit(2*i);
      }
    }
    //Xor w and r and save to w
    w->ClkXOR(r);
    w->copyToString(str, lenBloom+1);

    CLKout.push_back(string(str));
  }
  delete[] str;
  return CLKout;
}

vector<CLK*> CreateArmknechtCLK(int nrow, vector<string> IDc, vector<string> CLKin ,string password) {
  vector<CLK*> res;
  // random bit
  bool b;
  // compute length of Bloom Filter
  int lenBloom = 0;
  if (CLKin.size()>0)
    lenBloom = CLKin[0].size();
  else{
    Rcpp::Rcerr << "Missing input data.";
    //return 0result;
  }
  // Create CLK r and w
  CLK *r=  new CLK(lenBloom);
  r->init();
  CLK *w = new CLK(lenBloom);
  w->init();
  char *str = new char[lenBloom + 1];
  char id[100];

  //Generate lenBloom/2 randoms bit b dependened on password as seed
  seed_seq seed(password.begin(), password.end());
  default_random_engine engine(seed);
  vector<int> randomBits(lenBloom/2);
  uniform_int_distribution<int> distr(0, lenBloom - 1);
  generate(begin(randomBits), end(randomBits), [&]() {return distr(engine);});

  //for all lines
  for (int i =0; i < (int)CLKin.size() ; i++){
    //fill CLK r with input
    r->clear();
    strcpy(id, IDc[i].c_str()); //
    strcpy(str, CLKin[i].c_str());
    r->copyFromString(id, str);
    w->setId(id);
    // fill CLK w
    w->clear();
    for (int i =1; i < lenBloom/2 ; i++){
      //Choose a random bit b
      b=randomBits[i]%2;
      if (b==1){
        w->setBit(2*i-1);
        w->setBit(2*i);
      }
    }
    //Xor w and r and save to w
    w->ClkXOR(r);
    res.push_back(new CLK(w));
    }
  delete[] str;
  return res;
  }

/**
* Wrapper function for Compare
* @param d1_ encoded record 1
* @param d2_ encoded record 2
* @param t Tanomito similartiy to be tested
*/

float CompareArmknechtCLK(CLK* d1, CLK* d2, float t ) {
  float ctr=0; //counter, this value will track the estimate for HD(r1, r2).
  size_t h1 = d1->cardinality();
  size_t h2 = d2->cardinality();
  // Compute v = (v[1], . . . , v[n]) := d1 ⊕ d2. his yields the XOR of the two records plus a code word.
  //this is done within the if brackets

  for (int i =0; i < d1->getLength() ; i++){
    i++; //another i++, because two consecutive bit are compared as couples/pairs
    if(((d1->getBit(i))+(d2->getBit(i)))%2!= ((d1->getBit(i+1))+(d2->getBit(i+1)))%2) // A difference between the two records has been found for sure.
      ctr++;
  }
  return (ctr/(h1+h2));
}

size_t computeHW(string s){
  size_t hw = std::count(s.begin(), s.end(), '1');
  return hw;
}

