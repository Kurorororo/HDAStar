#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <utility>
#include <mpi.h>
#include "hdastar.h"
using namespace std;

HDAStar::HDAStar() {
  result = 255;
  count = 0;
  min = 0;
  tmax = 0;
  local_clock = 0;
  is_end = 0;
  message_box.time_stamp = 0;
  message_box.state = State();
  for (int i=0; i<255; ++i) {
    open[i].reserve(10000);
  }

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &node_size);
  createBuffer();
}

void HDAStar::createBuffer() {
  int b_size = sizeof(message_box);
  b_size += MPI_BSEND_OVERHEAD;
  if ((buff = (Message*)malloc(b_size)) == NULL) {
    cout << "\nInsufficient memory for Bsend buffer" << endl;
    fflush(NULL);
    exit(1);
  }
  MPI_Buffer_attach(buff, b_size);
}

int HDAStar::updateMin() {
  if (!open[min].empty())
    return 1;

  for (uint8_t i=min+1; i<255; ++i) {
    if (!open[i].empty()) {
      min = i;
      return 1;
    }
  }
  return 0;
}

void HDAStar::expand(State *s) {
  closed[s->tiles] = s->g;
  open[min].pop_back();
  if (s->blank >= State::WIDTH) {
    checkKid(s->makeKid(s->blank-State::WIDTH));
  }
  if (s->blank % State::WIDTH > 0) {
    checkKid(s->makeKid(s->blank-1));
  }
  if (s->blank % State::WIDTH < State::WIDTH-1) {
    checkKid(s->makeKid(s->blank+1));
  }
  if (s->blank < State::PUZZLE_SIZE-State::WIDTH) {
    checkKid(s->makeKid(s->blank+State::WIDTH));
  }
  State::last(s);
}

bool HDAStar::isClosed(const uint64_t &tiles, uint8_t g) {
  if (closed.find(tiles) == closed.end())
    return false;
  return closed[tiles] <= g;
}

void HDAStar::checkKid(State *kid) {
  if (isClosed(kid->tiles, kid->g))
    return;

  if (kid->isGoal()) {
    result = kid->g;
    cout << int(result) << endl;
    return;
  }

  int dest = kid->hash % node_size;
  if (dest == rank) {
    if (kid->f() < min)
      min = kid->f();
    open[kid->f()].push_back(kid);
    return;
  }

  sendMessage(dest, kid);
}

void HDAStar::sendMessage(int dest, State *s) {
  message_box.time_stamp = time(NULL);
  message_box.state = *s;
  MPI_Bsend(&message_box, sizeof(message_box), MPI_BYTE, dest, MESSAGE_TAG, MPI_COMM_WORLD);
  ++count;
  closed[s->tiles] = s->g;
  State::last(s);
}

void HDAStar::recieveMessage() {
  int flag;
  MPI_Status status;
  MPI_Iprobe(MPI_ANY_SOURCE, MESSAGE_TAG, MPI_COMM_WORLD, &flag, &status);
  while (flag) {
    MPI_Recv(&message_box, sizeof(message_box), MPI_BYTE, MPI_ANY_SOURCE, MESSAGE_TAG, MPI_COMM_WORLD, &status);
    --count;
    tmax = time(NULL);
    if (tmax < message_box.time_stamp)
      tmax = message_box.time_stamp;

    State *s = State::clone(message_box.state);
    if (isClosed(s->tiles, s->g))
      return;
    if (s->f() < min)
      min = s->f();
    open[s->f()].push_back(s);
    MPI_Iprobe(MPI_ANY_SOURCE, MESSAGE_TAG, MPI_COMM_WORLD, &flag, &status);
  }
}

void HDAStar::sendControl() {
  local_clock += 1;
  control_box.time_stamp = local_clock;
  control_box.accu = count;
  control_box.invalid = 0;
  control_box.init = rank;
  control_box.result = result;
  MPI_Bsend(&control_box, sizeof(control_box), MPI_BYTE,  (rank+1) % node_size, CONTROL_TAG, MPI_COMM_WORLD);
}

int HDAStar::checkResult(uint8_t result) {
  for (uint8_t i=0; i<result; ++i) {
    if (!open[i].empty()) {
      for (auto s : open[i])
        if (s->g < result)
          return 0;
    }
  }
  return 1;
}

int HDAStar::recieveControl() {
  int flag;
  MPI_Status status;
  MPI_Iprobe(MPI_ANY_SOURCE, CONTROL_TAG, MPI_COMM_WORLD, &flag, &status);
  while (flag) {
    MPI_Recv(&control_box, sizeof(control_box), MPI_BYTE, MPI_ANY_SOURCE, CONTROL_TAG, MPI_COMM_WORLD, &status);
    local_clock = time(NULL);
    if (local_clock < control_box.time_stamp)
      local_clock = control_box.time_stamp;

    if (control_box.init == rank) {
      if (control_box.accu == 0 && !control_box.invalid)
        is_end = 1;
      return 1;
    } else {
      control_box.accu += count;
      control_box.invalid = control_box.invalid || tmax >= control_box.time_stamp
                          || !checkResult(result);
      MPI_Bsend(&control_box, sizeof(control_box), MPI_BYTE, (rank+1) % node_size, CONTROL_TAG, MPI_COMM_WORLD);
    }
  }
  return 0;
}

int HDAStar::solve(uint8_t initial_tiles[], uint8_t initial_blank) {
  if (rank == 0) {
    State *s = State::initial(initial_tiles, initial_blank);
    int dest = s->hash % node_size;

    if (dest == 0) {
      open[s->f()].push_back(s);
      min = s->f();
    } else {
      sendMessage(dest, s);
    }
  }
  while (true) {
    if(recieveControl()) {
      MPI_Bcast(&is_end, 1, MPI_INT, control_box.init, MPI_COMM_WORLD);
    }
    if (is_end)
      return result;

    recieveMessage();

    if (!updateMin()) {
      continue;
    }
    expand(open[min].back());

    if (result != 255) {
      sendControl();
    }
  }

  return -1;
}

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);
  // uint8_t tiles[] = {0, 5, 2, 3, 1, 4, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  uint8_t tiles[] = {0, 1, 9, 7, 11, 13, 5, 3, 14, 12, 4, 2, 8, 6, 10, 15};
  // uint8_t tiles[] = {0, 13, 2, 4, 12, 14, 6, 9, 15, 1, 10, 3, 11, 5, 8, 7};
  // int8_t tiles[] = {15, 10, 8, 3, 0, 6, 9, 5, 1, 14, 13, 11, 7, 2, 12, 4};
  // int8_t tiles[] = {7, 6, 8, 1, 11, 5, 14, 10, 3, 4, 9, 13, 15, 2, 0, 12};
  HDAStar solver;
  cout << solver.solve(tiles, 0) << endl;
  // cout << solver.solve(tiles, 4) << endl;
  // cout << solver.solve(tiles, 14) << endl;
  MPI_Finalize();
  exit(0);
}
