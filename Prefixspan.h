#ifndef _PREFIXSPAN_H_
#define _PREFIXSPAN_H_

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <strstream>
#include <cstdlib>
#include <cmath>
#include <cstring>

using namespace std;
int w=1;

typedef pair<unsigned int, vector<string> > Transaction; //<trajid <locations> >
typedef pair<unsigned int, vector<int>    > TransactionTime;

struct Pairdata {
  vector<Transaction> database;
  vector<TransactionTime> time;
  vector<unsigned int> indeces;
  void clear() {
    database.clear();
    indeces.clear();
  }
};

class Prefixspan {
  unsigned int min_sup;
  unsigned int max_pat;
  vector<string> pattern;

public:
  Prefixspan(unsigned int _min_sup, unsigned int _max_pat) : min_sup(_min_sup), max_pat(_max_pat){};
  void read(const string &_filename, Pairdata &pairdata);
  void print_pattern(Pairdata &projected);
  void run(const string &_filename);
  void project(Pairdata &projected);
};

#endif // _PREFIXSPAN_H
