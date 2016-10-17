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

const char State::get_tile(char index) {
  return get_tile(tiles, index);
}

const void State::make_kid(State *kid, long long &newtiles, char newblank) {
  kid->tiles = newtiles;
  kid->blank = newblank;
  kid->h = h;
  kid->h -= MANHATTAN_DIST[get_tile(kid->blank)][kid->blank];
  kid->h += MANHATTAN_DIST[kid->get_tile(blank)][blank];
  kid->g = g + 1;
}

const void State::print() {
  for (int i=0; i<HEIGHT; ++i) {
    for (int j=0; j<WIDTH; ++j) {
      if (get_tile(i*WIDTH + j) < 10)
        cout << " ";
      cout << to_string(get_tile(i*WIDTH + j)) << " ";
    }
    cout << endl;
  }
  cout << endl;
}

void State::initial(char initial_tiles[], char initial_blank) {
  tiles = 0;
  for (int i=0; i<PUZZLE_SIZE; ++i)
    tiles = tiles << 4 | initial_tiles[i];
  blank = initial_blank;
  h = 0;
  g = 0;
  for (int i=0; i<PUZZLE_SIZE; i++) {
    h += MANHATTAN_DIST[get_tile(i)][i];
  }
}

const char State::get_tile(long long &tiles, char index) {
  return char(
    (tiles >> 4*(15-index)) &
    (long long)(15)
  );
}

void State::insert_blank(long long &tiles, char index) {
  tiles = tiles & (~((long long)(15)<<4*(15-index)));
}

void State::insert_tile(long long &tiles, char index, char value) {
  insert_blank(tiles, index);
  tiles = tiles | ((long long)(value)<<4*(15-index));
}
