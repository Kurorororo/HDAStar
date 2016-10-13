#include "state.cpp"

class AStar {
private:
  std::vector<State*> open[255];
  std::unordered_map<long long, bool> closed;
  unsigned char min;
  unsigned char result;
  long long tiles;
  const long long goal = 81985529216486895;

  void update_min();
  void expand(State*);
  void check_kid(State*, char);

public:
  AStar();
  int solve(char[], char);
};
