#include <vector>

class State {
public:
  static const int PUZZLE_SIZE = 16;
  static const int WIDTH = 4;
  static const int HEIGHT = 4;

  unsigned char h;
  unsigned char g;
  char blank;
  bool is_goal;
  char tiles[16];

  State();

  void initial(char[], char);
  inline const unsigned char f();
  const void make_kid(State*, char);
  inline const long long state_to_64bit();
  const void print();
};
