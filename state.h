#include <vector>

class State {
private:
  inline const char get_tile(char);
  void insert_blank(char);
  void insert_tile(char, char);
public:
  static const int PUZZLE_SIZE = 16;
  static const int WIDTH = 4;
  static const int HEIGHT = 4;

  unsigned char h;
  unsigned char g;
  char blank;
  bool is_goal;
  long long tiles;

  State();

  void initial(char[], char);
  inline const unsigned char f();
  const void make_kid(State*, char);
  const void print();
};
