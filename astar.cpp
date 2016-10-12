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
  result = -1;
}

void AStar::update_min() {
  if (open[min].empty()) {
    for(int i=0; i<200; ++i){
      if (!open[i].empty()) {
        min = i;
        break;
      }
     }

    if (open[min].empty()) {
      min = -1;
    }
  }
}

void AStar::distribute_kids(vector<State*> &temp) {
  auto itr = temp.begin();
  while (itr != temp.end()) {
    if(closed[(*itr)->state_to_string()] > 0) {
      Spool.destroy(*itr);
      itr = temp.erase(itr);
      continue;
    }
    if ((*itr)->is_goal) {
      result = (*itr)->g;
    }
    if ((*itr)->f() < min) {
      min = (*itr)->f();
    }
    open[(*itr)->f()].push_back(*itr);
    itr++;
  }
}

int AStar::solve(char initial_tiles[], char initial_blank) {
  auto compare = [](State* l, State* r){ return l->g > r->g; };
  State* s = Spool.construct();
  s->initial(initial_tiles, initial_blank);
  open[s->f()].push_back(s);

  min = s->f();
  vector<State*> temp;
  int counter = 0;

  while (true) {
    update_min();
    if (min < 0)
      return -1;

    closed[open[min].back()->state_to_string()] += 1;
    open[min].back()->expand(temp);
    Spool.destroy(open[min].back());
    open[min].pop_back();

    distribute_kids(temp);
    if (result > 0)
      return result;
    temp.clear();
    counter++;
  }

  cout << counter << endl;
  return -1;
}

int main() {
  // char tiles[] = {0, 1, 9, 7, 11, 13, 5, 3, 14, 12, 4, 2, 8, 6, 10, 15};
  // char tiles[] = {0, 13, 2, 4, 12, 14, 6, 9, 15, 1, 10, 3, 11, 5, 8, 7};
  char tiles[] = {7, 6, 8, 1, 11, 5, 14, 10, 3, 4, 9, 13, 15, 2, 0, 12};
  AStar solver;
  cout << solver.solve(tiles, 14) << endl;
}
