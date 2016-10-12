#include <queue>
#include "state.cpp"

class AStar {
private:
  std::vector<State*> open[200];
  std::unordered_map<std::string, char> closed;
  int min;
  int result;

  void update_min();
  void distribute_kids(std::vector<State*>&);

public:
  AStar();
  int solve(char[], char);
};
