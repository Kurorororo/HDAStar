#include <boost/pool/object_pool.hpp>
#include "../common/manhattan_dist.h"

class State {
public:
  static const int PUZZLE_SIZE = 16;
  static const int WIDTH = 4;
  static const int HEIGHT = 4;
  static const int64_t GOAL = 81985529216486895;
  static std::vector< std::vector<uint8_t> > h_table;

  State();
  State(const State&);

  static State* initial(uint8_t[], uint8_t);
  static State* clone(const State&);
  static void last(State*);
  const uint8_t f();
  const uint8_t getTile(uint8_t);
  void insertBlank(uint8_t);
  void insertTile(uint8_t, uint8_t);
  bool isGoal();
  State* makeKid(uint8_t);
  const void print();

  uint64_t tiles;
  uint8_t blank;
  uint8_t h;
  uint8_t g;
  uint64_t parent;
};

namespace {
  boost::object_pool<State> Spool(100000);
}
