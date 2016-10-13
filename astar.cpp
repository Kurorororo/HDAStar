#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include "astar.h"
using namespace std;

#if 0
goal_state =
{  0,  1,  2,  3,
   4,  5,  6,  7,
   8,  9, 10, 11,
  12, 13, 14, 15
}
#endif

AStar::AStar() {
  result = 255;
}

void AStar::update_min() {
  if (open[min].empty()) {
    for(unsigned char i=0; i<255; ++i){
      if (!open[i].empty()) {
        min = i;
        break;
      }
     }

    if (open[min].empty()) {
      min = 255;
    }
  }
}

void AStar::expand(State* s) {
  open[min].pop_back();
  if (s->blank >= State::WIDTH) {
    State* kid1 = Spool.construct();
    s->make_kid(kid1, s->blank-State::WIDTH);
    check_kid(kid1);
  }
  if (s->blank % State::WIDTH > 0) {
    State* kid2= Spool.construct();
    s->make_kid(kid2, s->blank-1);
    check_kid(kid2);
  }
  if (s->blank % State::WIDTH < State::WIDTH-1) {
    State* kid3 = Spool.construct();
    s->make_kid(kid3, s->blank+1);
    check_kid(kid3);
  }
  if (s->blank < State::PUZZLE_SIZE-State::WIDTH) {
    State* kid4 = Spool.construct();
    s->make_kid(kid4, s->blank+State::WIDTH);
    check_kid(kid4);
  }
  Spool.destroy(s);
}

void AStar::check_kid(State* kid) {
  if (kid->is_goal) {
    result = kid->g;
    return;
  }
  if (closed[kid->state_to_64bit()]) {
    Spool.destroy(kid);
    return;
  }
  open[kid->f()].push_back(kid);
}

int AStar::solve(char initial_tiles[], char initial_blank) {
  // auto compare = [](State* l, State* r){ return l->g > r->g; };
  State* s = Spool.construct();
  s->initial(initial_tiles, initial_blank);
  open[s->f()].push_back(s);

  min = s->f();

  while (true) {
    update_min();
    if (min == 255)
      return -1;

    closed[open[min].back()->state_to_64bit()] = true;
    expand(open[min].back());

    if (result != 255)
      return result;
  }

  return -1;
}

int main() {
  // char tiles[] = {0, 1, 9, 7, 11, 13, 5, 3, 14, 12, 4, 2, 8, 6, 10, 15};
  char tiles[] = {0, 13, 2, 4, 12, 14, 6, 9, 15, 1, 10, 3, 11, 5, 8, 7};
  // char tiles[] = {7, 6, 8, 1, 11, 5, 14, 10, 3, 4, 9, 13, 15, 2, 0, 12};
  AStar solver;
  cout << solver.solve(tiles, 0) << endl;
}
