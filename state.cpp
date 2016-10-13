#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/pool/object_pool.hpp>
#include "state.h"
using namespace std;

namespace {
  unsigned char MANHATTAN_DIST[16][16] = {
    {0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0},
    {1 ,0 ,1 ,2 ,2 ,1 ,2 ,3 ,3 ,2 ,3 ,4 ,4 ,3 ,4 ,5},
    {2 ,1 ,0 ,1 ,3 ,2 ,1 ,2 ,4 ,3 ,2 ,3 ,5 ,4 ,3 ,4},
    {3 ,2 ,1 ,0 ,4 ,3 ,2 ,1 ,5 ,4 ,3 ,2 ,6 ,5 ,4 ,3},
    {1 ,2 ,3 ,4 ,0 ,1 ,2 ,3 ,1 ,2 ,3 ,4 ,2 ,3 ,4 ,5},
    {2 ,1 ,2 ,3 ,1 ,0 ,1 ,2 ,2 ,1 ,2 ,3 ,3 ,2 ,3 ,4},
    {3 ,2 ,1 ,2 ,2 ,1 ,0 ,1 ,3 ,2 ,1 ,2 ,4 ,3 ,2 ,3},
    {4 ,3 ,2 ,1 ,3 ,2 ,1 ,0 ,4 ,3 ,2 ,1 ,5 ,4 ,3 ,2},
    {2 ,3 ,4 ,5 ,1 ,2 ,3 ,4 ,0 ,1 ,2 ,3 ,1 ,2 ,3 ,4},
    {3 ,2 ,3 ,4 ,2 ,1 ,2 ,3 ,1 ,0 ,1 ,2 ,2 ,1 ,2 ,3},
    {4 ,3 ,2 ,3 ,3 ,2 ,1 ,2 ,2 ,1 ,0 ,1 ,3 ,2 ,1 ,2},
    {5 ,4 ,3 ,2 ,4 ,3 ,2 ,1 ,3 ,2 ,1 ,0 ,4 ,3 ,2 ,1},
    {3 ,4 ,5 ,6 ,2 ,3 ,4 ,5 ,1 ,2 ,3 ,4 ,0 ,1 ,2 ,3},
    {4 ,3 ,4 ,5 ,3 ,2 ,3 ,4 ,2 ,1 ,2 ,3 ,1 ,0 ,1 ,2},
    {5 ,4 ,3 ,4 ,4 ,3 ,2 ,3 ,3 ,2 ,1 ,2 ,2 ,1 ,0 ,1},
    {6 ,5 ,4 ,3 ,5 ,4 ,3 ,2 ,4 ,3 ,2 ,1 ,3 ,2 ,1 ,0}
  };

  boost::object_pool<State> Spool(100000);
}

State::State() {}

const unsigned char State::f() {
  return h + g;
}

const void State::make_kid(State *kid, char newblank) {
  for (int i=0; i<PUZZLE_SIZE; ++i)
    kid->tiles[i] = tiles[i];
  kid->blank = newblank;
  kid->tiles[blank] = tiles[kid->blank];
  kid->h = h;
  kid->h -= MANHATTAN_DIST[tiles[kid->blank]][kid->blank];
  kid->h += MANHATTAN_DIST[kid->tiles[blank]][blank];
  if (kid->h == 0) {
    kid->is_goal = true;
  } else {
    kid->is_goal = false;
  }
  kid->g = g + 1;
}

const long long State::state_to_64bit() {
  long long a = 0;
  for (int i=0; i<PUZZLE_SIZE; ++i) {
    a = (a << 4) | tiles[i];
  }
  return a;
}

const void State::print() {
  for (int i=0; i<HEIGHT; ++i) {
    for (int j=0; j<WIDTH; ++j) {
      if (tiles[i*WIDTH + j] < 10)
        cout << " ";
      cout << to_string(tiles[i*WIDTH + j]) << " ";
    }
    cout << endl;
  }
  cout << endl;
}

void State::initial(char initial_tiles[], char initial_blank) {
  for (int i=0; i<PUZZLE_SIZE; ++i)
    tiles[i] = initial_tiles[i];
  tiles[initial_blank] = 0;
  blank = initial_blank;
  is_goal = false;
  h = 0;
  g = 0;
  for (int i=0; i<PUZZLE_SIZE; i++)
    h += MANHATTAN_DIST[tiles[i]][i];
}
