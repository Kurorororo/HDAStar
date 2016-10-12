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
  if (open[min].size() == 0) {
    for(auto &pair:open){
      if (pair.second.size() > 0) {
        min = pair.first;
        break;
      }
     }

    if (open[min].size() == 0) {
      min = -1;
    }
  }
}

void AStar::distribute_kids(vector<State*> &temp) {
  auto itr = temp.begin();
  while (itr != temp.end()) {
    if(closed[(*itr)->state_to_string()] > 0) {
      delete(*itr);
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

int AStar::solve(char initial_tiles[], int initial_blank) {
  State* s = new State(initial_tiles, 0);
  s->initial();
  open[s->f()].push_back(s);

  min = s->f();
  vector<State*> temp;
  int counter = 0;

  while (true) {
    update_min();
    if (min < 0)
      return -1;

    State* fringe = open[min].back();
    open[min].pop_back();
    closed[fringe->state_to_string()] += 1;
    fringe->expand(temp);
    delete(fringe);

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
  char tiles[] = {0, 13, 2, 4, 12, 14, 6, 9, 15, 1, 10, 3, 11, 5, 8, 7};
  // vector<int> tiles{15, 2, 12, 11, 14, 13, 9, 5, 1, 3, 8, 7, 0, 10, 6, 4};
  AStar solver;
  cout << solver.solve(tiles, 0) << endl;
}
