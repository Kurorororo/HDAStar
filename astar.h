#include "state.cpp"

class AStar {
private:
  std::map<int, std::vector<State*> > open;
  std::unordered_map<std::string, int> closed;
  int min;
  int result;

  void update_min();
  void distribute_kids(std::vector<State*>&);

public:
  AStar();
  int solve(char[], int);
};
