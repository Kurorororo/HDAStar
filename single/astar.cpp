#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include "astar.h"
using namespace std;

AStar::AStar() {
  result = 255;
  min = 0;
  is_end = 0;
  for (int i=0; i<255; ++i)
    open[i].reserve(10000);
  closed.reserve(100000);

}

int AStar::updateMin() {
  if (min >= result)
    return 0;

  if (!open[min].empty())
    return 1;

  for (uint8_t i=min+1; i<255; ++i) {
    if (!open[i].empty()) {
      min = i;
      return 1;
    }
  }
  return 0;
}

void AStar::expand(State *s) {
  open[min].pop_back();
  closed[s->tiles] = make_pair(s->g, s->tiles);
  if (s->blank >= State::WIDTH) {
    checkKid(s->makeKid(s->blank-State::WIDTH));
  }
  if (s->blank % State::WIDTH > 0) {
    checkKid(s->makeKid(s->blank-1));
  }
  if (s->blank % State::WIDTH < State::WIDTH-1) {
    checkKid(s->makeKid(s->blank+1));
  }
  if (s->blank < State::PUZZLE_SIZE-State::WIDTH) {
    checkKid(s->makeKid(s->blank+State::WIDTH));
  }
  State::last(s);
}

int AStar::isClosed(const uint64_t &tiles, uint8_t g) {
  if (closed.find(tiles) == closed.end())
    return 0;
  if (closed[tiles].first <= g)
    return 1;
  return 0;
}

void AStar::checkKid(State *kid) {
  if (isClosed(kid->tiles, kid->g))
    return;

  if (kid->isGoal()) {
    is_end = 1;
    result = kid->g;
    min = kid->g;
    return;
  }

  if (kid->f() < min)
    min = kid->f();
  open[kid->f()].push_back(kid);
}

int AStar::solve(uint8_t initial_tiles[], uint8_t initial_blank) {
  State *s = State::initial(initial_tiles, initial_blank);
  open[s->f()].push_back(s);
  min = s->f();

  while (true) {
    if (is_end)
      return result;

    if (!updateMin())
      continue;
    expand(open[min].back());
  }
  return -1;
}

int main(int argc, char *argv[]) {
  int blank;
  cin >> blank;
  int in_tile;
  uint8_t tiles[16];
  for (int i=0; i<16; ++i) {
    cin >> in_tile;
    tiles[i] = in_tile;
  }

  AStar solver;
  int result = solver.solve(tiles, static_cast<uint8_t>(blank));
  cout << result << endl;
  exit(0);
}
