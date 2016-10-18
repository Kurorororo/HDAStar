#include "state.cpp"

class AStar {
private:
  std::vector<State*> open[255];
  std::unordered_map<int64_t, bool> closed;
  uint8_t min;
  uint8_t result;
  int64_t tiles;
  const int64_t goal = 81985529216486895;

  void update_min();
  void expand(State*);
  void check_kid(State*, int8_t);

public:
  AStar();
  int solve(int8_t[], int8_t);
};
