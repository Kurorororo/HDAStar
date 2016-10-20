#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/pool/object_pool.hpp>
#include "state.h"
#include "const.h"
using namespace std;

State::State(uint8_t initial_blank) {
  tiles = 0;
  blank = initial_blank;
  h = 0;
  g = 0;
  hash = 0;
}

State::State(const State &s) {
  tiles = s.tiles;
  blank = s.blank;
  h = s.h;
  g = s.g;
  hash = s.hash;
}

State* State::initial(uint8_t initial_tiles[], uint8_t initial_blank) {
  State *s = Spool.construct(initial_blank);
  for (int i=0; i<PUZZLE_SIZE; ++i) {
    s->tiles = s->tiles << 4 | initial_tiles[i];
    s->hash = s->hash ^ ZOBRIST_HASH[initial_tiles[i]][i];
    s->h += MANHATTAN_DIST[initial_tiles[i]][i];
  }
  s->hash = s->hash ^ G_HASH[s->g];
  return s;
}

State* State::clone(const State &s) {
  State *t = Spool.construct(s);
  return t;
}

void State::last(State* s) {
  Spool.destroy(s);
}

const uint8_t State::f() {
  return h + g;
}

const uint8_t State::getTile(uint8_t index) {
  return int8_t(
    (tiles >> 4*(15-index)) &
    (int64_t)(15)
  );
}

void State::insertBlank(uint8_t index) {
  tiles = tiles & (~((int64_t)(15)<<4*(15-index)));
}

void State::insertTile(uint8_t index, uint8_t value) {
  insertBlank(index);
  tiles = tiles | ((int64_t)(value)<<4*(15-index));
}

bool State::isGoal() {
  return tiles == GOAL;
}

State* State::makeKid(uint8_t newblank) {
  State *kid = Spool.construct(newblank);
  kid->tiles = tiles;
  kid->insertBlank(newblank);
  kid->insertTile(blank, getTile(newblank));
  kid->h = h;
  kid->h -= MANHATTAN_DIST[getTile(kid->blank)][kid->blank];
  kid->h += MANHATTAN_DIST[kid->getTile(blank)][blank];
  kid->g = g + 1;
  kid->hash = hash ^ ZOBRIST_HASH[getTile(newblank)][newblank];
  kid->hash = hash ^ ZOBRIST_HASH[getTile(newblank)][blank];
  kid->hash = hash ^ G_HASH[g];
  kid->hash = hash ^ G_HASH[g+1];
  return kid;
}

const void State::print() {
  for (int i=0; i<HEIGHT; ++i) {
    for (int j=0; j<WIDTH; ++j) {
      if (getTile(i*WIDTH + j) < 10)
        cout << " ";
      cout << to_string(getTile(i*WIDTH + j)) << " ";
    }
    cout << endl;
  }
  cout << endl;
}
