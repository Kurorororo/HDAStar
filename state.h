class State {
public:
  static const int PUZZLE_SIZE = 16;
  static const int WIDTH = 4;
  static const int HEIGHT = 4;

  unsigned char h;
  unsigned char g;
  char blank;
  long long tiles;

  State();

  void initial(char[], char);
  static const char get_tile(long long&, char);
  static void insert_blank(long long&, char);
  static void insert_tile(long long&, char, char);
  inline const unsigned char f();
  inline const char get_tile(char);
  const void make_kid(State*, long long&, char);
  const void print();
};
