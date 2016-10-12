#include <iostream>
#include <vector>
#include "manhattan_calculater.h"
using namespace std;

ManhattanCalculater::ManhattanCalculater(
  int initial_puzzle_size, int initial_width
) {
  puzzle_size = initial_puzzle_size;
  width = initial_width;
  table = vector< vector<int> >(puzzle_size, vector<int>(puzzle_size));
}

void ManhattanCalculater::int_to_xy(int value, int xy[2]) {
  xy[0] = value % width;
  xy[1] = value / width;
}

const int ManhattanCalculater::calc_manhattan_dist(int value, int position) {
  if (value == 0)
    return 0;

  int value_xy[2];
  int_to_xy(value, value_xy);
  int position_xy[2];
  int_to_xy(position, position_xy);
  int x_dist = position_xy[0] - value_xy[0];
  if (x_dist < 0) {
    x_dist = -x_dist;
  }
  int y_dist = position_xy[1] - value_xy[1];
  if (y_dist < 0) {
    y_dist = -y_dist;
  }
  return x_dist + y_dist;
}

void ManhattanCalculater::calc_manhattan_dists() {
  for (int i=0; i<puzzle_size; ++i)
    for (int j=0; j<puzzle_size; ++j)
      table[i][j] = calc_manhattan_dist(i, j);

  cout << "{" << endl;
  for (int i=0; i<puzzle_size; ++i) {
    cout << "  {";
    for(int j=0; j<puzzle_size; j++) {
      cout << table[i][j];
      if (j != puzzle_size-1)
        cout << " ,";
    }
    cout << "}";
    if (i != puzzle_size-1)
      cout << ",";
    cout << endl;
  }
  cout << "}" << endl;
}

int main() {
  ManhattanCalculater calculater(16, 4);
  calculater.calc_manhattan_dists();
}
