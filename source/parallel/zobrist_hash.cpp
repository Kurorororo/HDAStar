#include <iostream>
#include <vector>
#include <random>
#include "zobrist_hash.h"
using namespace std;

vector< vector<uint64_t> > ZobristHash::calculate(int field_size, int characters) {
  vector< vector<uint64_t> > table(characters, vector<uint64_t>(field_size));
  random_device rnd;
  mt19937_64 mt(rnd());
  for (int i=0; i<characters; ++i) {
    for (int j=0; j<field_size; ++j) {
      if (i == 0)
        table[i][j] = 0;
      else
        table[i][j] = mt();
    }
  }
  return table;
}
