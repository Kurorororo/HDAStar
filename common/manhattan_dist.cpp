#include <iostream>
#include <vector>
#include "manhattan_dist.h"
using namespace std;

void ManhattanDist::intToXy(int value, int xy[2], int width) {
  xy[0] = value % width;
  xy[1] = value / width;
}

const int ManhattanDist::calculatePartial(int value, int position, int width) {
  if (value == 0)
    return 0;

  int value_xy[2];
  intToXy(value, value_xy, width);
  int position_xy[2];
  intToXy(position, position_xy, width);
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

vector< vector<uint8_t> > ManhattanDist::calculate(int puzzle_size, int width) {
  vector< vector<uint8_t> > table(puzzle_size, vector<uint8_t>(width));
  for (int i=0; i<puzzle_size; ++i)
    for (int j=0; j<puzzle_size; ++j)
      table[i][j] = calculatePartial(i, j, width);

  cout << "{" << endl;
  for (int i=0; i<puzzle_size; ++i) {
    cout << "  {";
    for(int j=0; j<puzzle_size; j++) {
      cout << int(table[i][j]);
      if (j != puzzle_size-1)
        cout << " ,";
    }
    cout << "}";
    if (i != puzzle_size-1)
      cout << ",";
    cout << endl;
  }
  cout << "}" << endl;

  return table;
}

int main() {
  ManhattanDist::calculate(16, 4);
}
