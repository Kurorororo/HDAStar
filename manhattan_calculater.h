class ManhattanCalculater {
private:
  int puzzle_size;
  int width;
  void int_to_xy(int, int[]);
  const int calc_manhattan_dist(int, int);
public:
  std::vector< std::vector<int> > table;
  ManhattanCalculater(int, int);
  void calc_manhattan_dists();
};
