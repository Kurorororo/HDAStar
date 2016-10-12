#include <iostream>
#include "state.h"
using namespace std;

namespace {
  int MANHATTAN_DIST[16][16] = {
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
}

State::State(char initial_tiles[], char initial_blank) {
  for (int i=0; i<PUZZLE_SIZE; ++i)
    tiles[i] = initial_tiles[i];
  tiles[initial_blank] = 0;
  blank = initial_blank;
  is_goal = false;
  h = 0;
  g = 0;
}

const int State::f() {
  return h + g;
}

const void State::make_kid(State *kid) {
  kid->h = h;
  kid->tiles[blank] = tiles[kid->blank];
  kid->h -= MANHATTAN_DIST[tiles[kid->blank]][kid->blank];
  kid->h += MANHATTAN_DIST[kid->tiles[blank]][blank];
  if (kid->h == 0) {
    kid->is_goal = true;
  }
  kid->g = g + 1;
}

const void State::expand(vector<State*> &kids) {
  if (blank >= WIDTH) {
    State* kid1 = new State(tiles, blank-WIDTH);
    make_kid(kid1);
    kids.push_back(kid1);
  }
  if (blank % WIDTH > 0) {
    State* kid2 = new State(tiles, blank-1);
    make_kid(kid2);
    kids.push_back(kid2);
  }
  if (blank % WIDTH < WIDTH-1) {
    State* kid3 = new State(tiles, blank+1);
    make_kid(kid3);
    kids.push_back(kid3);
  }
  if (blank < PUZZLE_SIZE-WIDTH) {
    State* kid4 = new State(tiles, blank+WIDTH);
    make_kid(kid4);
    kids.push_back(kid4);
  }
}

const string State::state_to_string() {
  string s(tiles, end(tiles));
  return s;
}

const void State::print() {
  for (int i=0; i<HEIGHT; ++i) {
    for (int j=0; j<WIDTH; ++j) {
      if (tiles[i*WIDTH + j] < 10)
        cout << " ";
      cout << tiles[i*WIDTH + j] << " ";
    }
    cout << endl;
  }
  cout << endl;
}

void State::initial() {
  for (int i=0; i<PUZZLE_SIZE; i++)
    h += MANHATTAN_DIST[tiles[i]][i];
}
