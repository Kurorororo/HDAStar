#include <queue>
#include "state.cpp"

class AStar {
private:
  std::vector<State*> open[255];
  std::unordered_map<long long, bool> closed;
  unsigned char min;
  unsigned char result;

  void update_min();
  void expand(State*);
  void check_kid(State*);

public:
  AStar();
  int solve(char[], char);
};
