#include "state.h"

class AStar {
 public:
  AStar();
  int solve(uint8_t[], uint8_t);

 private:
  static const int MESSAGE_TAG = 0;
  static const int CONTROL_TAG = 1;

  int updateMin();
  void expand(State*);
  int isClosed(const uint64_t&, uint8_t);
  void checkKid(State*);

  std::vector<State*> open[255];
  std::unordered_map<uint64_t, std::pair<uint8_t, uint64_t> > closed;
  uint8_t min;
  uint8_t result;
  int is_end;
};
