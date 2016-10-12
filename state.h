class State {
private:
  static const int PUZZLE_SIZE = 16;
  static const int WIDTH = 4;
  static const int HEIGHT = 4;
public:
  int h;
  int g;
  char blank;
  bool is_goal;
  char tiles[16];

  State(char[], char);

  void initial();
  inline const int f();
  const void make_kid(State*);
  const void expand(std::vector<State*>&);
  inline const std::string state_to_string();
  const void print();
};
