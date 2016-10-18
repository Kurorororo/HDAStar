#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/pool/object_pool.hpp>
#include "state.h"
#include "const.h"
using namespace std;

State::State() {}

const uint8_t State::f() {
  return h + g;
}

const int8_t State::get_tile(int8_t index) {
  return get_tile(tiles, index);
}

const void State::make_kid(State *kid, int64_t &newtiles, int8_t newblank) {
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

void State::initial(int8_t initial_tiles[], int8_t initial_blank) {
  tiles = 0;
  h = 0;
  g = 0;
  blank = initial_blank;
  for (int i=0; i<PUZZLE_SIZE; ++i) {
    tiles = tiles << 4 | initial_tiles[i];
    hash = hash ^ ZOBRIST_HASH[initial_tiles[i]][i];
    h += MANHATTAN_DIST[get_tile(i)][i];
  }
}

void State::initial_hash() {
  hash = 0;
  for (int i=0; i<PUZZLE_SIZE; ++i) {
    hash = hash ^ ZOBRIST_HASH[get_tile(i)][i];
  }
}

const int8_t State::get_tile(int64_t &tiles, int8_t index) {
  return int8_t(
    (tiles >> 4*(15-index)) &
    (int64_t)(15)
  );
}

void State::insert_blank(int64_t &tiles, int8_t index) {
  tiles = tiles & (~((int64_t)(15)<<4*(15-index)));
}

void State::insert_tile(int64_t &tiles, int8_t index, int8_t value) {
  insert_blank(tiles, index);
  tiles = tiles | ((int64_t)(value)<<4*(15-index));
}
