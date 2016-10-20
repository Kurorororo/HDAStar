#include <iostream>
#include <random>
#include <unordered_map>
#include "zobrist_hash.h"
using namespace std;

ZobristHash::ZobristHash(int f, int c) {
  field_size = f;
  characters = c;
  table = vector< vector<uint64_t> >(field_size, vector<uint64_t>(characters));
}

void ZobristHash::calc_zobrist_hash() {
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

  cout << "{" << endl;
  for (int i=0; i<characters; ++i) {
    cout << "  {" << endl;
    for(int j=0; j<field_size; j++) {
      cout << "    " << table[i][j];
      if (j != characters-1)
        cout << ",";
      cout << endl;
    }
    cout << "  }";
    if (i != field_size-1)
      cout << ",";
    cout << endl;
  }
  cout << "}" << endl;
}

void ZobristHash::n_length_randoms(int n) {
  random_device rnd;
  mt19937_64 mt(rnd());
  cout << "{" << endl;
  for (int i=0; i<n; ++i) {
    cout << "  " << mt();
    if (i != n)
      cout << ",";
    cout << endl;
  }
  cout << "}" << endl;
}

int main() {
  ZobristHash calculater(16, 16);
  calculater.calc_zobrist_hash();
  calculater.n_length_randoms(255);
}
