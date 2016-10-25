#include <iostream>
#include <vector>
#include "state.h"
using namespace std;

vector< vector<uint8_t> >State::h_table = ManhattanDist::calculate(16, 4);

State::State() {
  tiles = 0;
  blank = 0;
  h = 0;
  g = 0;
}

State::State(const State &s) {
  tiles = s.tiles;
  blank = s.blank;
  h = s.h;
  g = s.g;
}

State* State::initial(uint8_t initial_tiles[], uint8_t initial_blank) {
  State *s = Spool.construct();
  s->blank = initial_blank;
  for (int i=0; i<PUZZLE_SIZE; ++i) {
    s->tiles = s->tiles << 4 | initial_tiles[i];
    s->h += h_table[initial_tiles[i]][i];
  }
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
  State *kid = Spool.construct();
  kid->tiles = tiles;
  kid->blank = newblank;
  kid->insertBlank(newblank);
  kid->insertTile(blank, getTile(newblank));
  kid->h = h;
  kid->h -= h_table[getTile(kid->blank)][kid->blank];
  kid->h += h_table[kid->getTile(blank)][blank];
  kid->g = g + 1;
  kid->parent = tiles;
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
