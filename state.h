class State {
public:
  static const int PUZZLE_SIZE = 16;
  static const int WIDTH = 4;
  static const int HEIGHT = 4;

  uint8_t h;
  uint8_t g;
  int8_t blank;
  int64_t tiles;
  uint64_t hash;

  State();

  void initial(int8_t[], int8_t);
  void initial_hash();
  static const int8_t get_tile(int64_t&, int8_t);
  static void insert_blank(int64_t&, int8_t);
  static void insert_tile(int64_t&, int8_t, int8_t);
  inline const uint8_t f();
  inline const int8_t get_tile(int8_t);
  const void make_kid(State*, int64_t&, int8_t);
  const void print();
};
