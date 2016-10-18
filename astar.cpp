#include <iostream>
#include <vector>
#include <unordered_map>
#include "astar.h"
using namespace std;

AStar::AStar() {
  result = 255;
  for (int i=0; i<255; ++i)
    open[i].reserve(10000);
  closed.reserve(100000);
}

void AStar::update_min() {
  if (!open[min].empty())
    return;

  for (uint8_t i=min+1; i<255; ++i) {
    if (!open[i].empty()) {
      min = i;
      return;
    }
  }

  min = 255;
}

void AStar::expand(State* s) {
  open[min].pop_back();
  if (s->blank >= State::WIDTH) {
    check_kid(s, s->blank-State::WIDTH);
  }
  if (s->blank % State::WIDTH > 0) {
    check_kid(s, s->blank-1);
  }
  if (s->blank % State::WIDTH < State::WIDTH-1) {
    check_kid(s, s->blank+1);
  }
  if (s->blank < State::PUZZLE_SIZE-State::WIDTH) {
    check_kid(s, s->blank+State::WIDTH);
  }
  Spool.destroy(s);
}

void AStar::check_kid(State* s, int8_t newblank) {
  tiles = s->tiles;
  State::insert_tile(tiles, s->blank, s->get_tile(newblank));
  State::insert_blank(tiles, newblank);
  if (closed[tiles])
    return;
  if (tiles == goal) {
    result = s->g+1;
    return;
  }

  State* kid = Spool.construct();
  s->make_kid(kid, tiles, newblank);
  if (kid->f() < min)
    min = kid->f();
  open[kid->f()].push_back(kid);
}

int AStar::solve(int8_t initial_tiles[], int8_t initial_blank) {
  State* s = Spool.construct();
  s->initial(initial_tiles, initial_blank);
  open[s->f()].push_back(s);

  min = s->f();

  while (true) {
    update_min();
    if (min == 255)
      return -1;

    closed[open[min].back()->tiles] = true;
    expand(open[min].back());

    if (result != 255)
      return result;
  }

  return -1;
}

int main() {
  // int8_t tiles[] = {0, 1, 9, 7, 11, 13, 5, 3, 14, 12, 4, 2, 8, 6, 10, 15};
  int8_t tiles[] = {0, 13, 2, 4, 12, 14, 6, 9, 15, 1, 10, 3, 11, 5, 8, 7};
  // int8_t tiles[] = {15, 10, 8, 3, 0, 6, 9, 5, 1, 14, 13, 11, 7, 2, 12, 4};
  // int8_t tiles[] = {7, 6, 8, 1, 11, 5, 14, 10, 3, 4, 9, 13, 15, 2, 0, 12};
  AStar solver;
  cout << solver.solve(tiles, 0) << endl;
  // cout << solver.solve(tiles, 4) << endl;
  // cout << solver.solve(tiles, 14) << endl;
  exit(0);
}
