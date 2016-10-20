#include <iostream>
#include <vector>
#include <unordered_set>
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
  for (int i=0; i<255; ++i)
    open[i].reserve(10000);
  closed.reserve(100000);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &node_size);
  message_box = vector<Message>(node_size, Message{0, State(0)});
  createMessageStruct();
  createControlStruct();
  createBuffer();
}

void HDAStar::createMessageStruct() {
  MPI_Datatype types[5] = {
    MPI_UINT64_T,
    MPI_UINT8_T,
    MPI_UINT8_T,
    MPI_UINT8_T,
    MPI_UINT64_T
  };
  int counts[5] = {1, 1, 1, 1, 1};
  MPI_Aint positions[5], lb, ex;
  positions[0] = 0;
  MPI_Type_get_extent(types[0], &lb, &ex);
  positions[1] = ex;
  positions[2] = positions[1] + ex;
  positions[3] = positions[2] + ex;
  MPI_Type_get_extent(types[3], &lb, &ex);
  positions[4] = positions[3] + ex;
  MPI_Type_create_struct(5, counts, positions, types, &state_struct);
  MPI_Type_commit(&state_struct);

  MPI_Datatype message_types[2] = {
    MPI_UINT32_T,
    state_struct
  };
  int message_counts[2] = {1, 1};
  MPI_Aint message_positions[2];
  message_positions[0] = 0;
  MPI_Type_get_extent(message_types[0], &lb, &ex);
  message_positions[1] = ex;
  MPI_Type_create_struct(2, message_counts, message_positions, message_types, &message_struct);
  MPI_Type_commit(&message_struct);
  MPI_Type_get_extent(message_struct, &lb, &ex);
}

void HDAStar::createControlStruct() {
  MPI_Datatype types[5] = {
    MPI_INT32_T,
    MPI_INT,
    MPI_INT,
    MPI_INT,
    MPI_UINT8_T,
  };
  int counts[5] = {1, 1, 1, 1, 1};
  MPI_Aint positions[5], lb, ex;
  positions[0] = 0;
  MPI_Type_get_extent(types[0], &lb, &ex);
  positions[1] = ex;
  MPI_Type_get_extent(types[1], &lb, &ex);
  positions[2] = positions[1] + ex;
  positions[3] = positions[2] + ex;
  positions[4] = positions[3] + ex;
  MPI_Type_create_struct(5, counts, positions, types, &control_struct);
  MPI_Type_commit(&control_struct);
}

void HDAStar::createBuffer() {
  int b_size;
  MPI_Pack_size(node_size, message_struct, MPI_COMM_WORLD, &b_size);
  b_size += 6*MPI_BSEND_OVERHEAD;
  if ((buff = (Message*)malloc(b_size)) == NULL) {
    printf("\nInsufficient memory for Bsend buffer"); fflush(NULL);
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
  // s->print();
  closed.insert(s->hash);
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

void HDAStar::checkKid(State *kid) {
  if (!(closed.find(kid->hash) == closed.end()))
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
  ++count;
  message_box[dest].time_stamp = time(NULL);
  message_box[dest].state = *s;
  MPI_Bsend(&(message_box[dest]), 1, message_struct, dest, MESSAGE_TAG, MPI_COMM_WORLD);
  closed.insert(s->hash);
  State::last(s);
}

void HDAStar::recieveMessage() {
  int flag;
  MPI_Status status;
  MPI_Iprobe(MPI_ANY_SOURCE, MESSAGE_TAG, MPI_COMM_WORLD, &flag, &status);
  if (!flag)
    return;

  int message_size;
  MPI_Get_count(&status, message_struct, &message_size);
  Message m = {0, State(0)};
  MPI_Recv(&(message_box[rank]), message_size, message_struct, MPI_ANY_SOURCE, MESSAGE_TAG, MPI_COMM_WORLD, &status);

  --count;
  tmax = time(NULL);
  if (tmax < message_box[rank].time_stamp)
    tmax = message_box[rank].time_stamp;

  State *s = State::clone(message_box[rank].state);
  if (!(closed.find(s->hash) == closed.end()))
    return;
  if (s->f() < min)
    min = s->f();
  open[s->f()].push_back(s);
}

void HDAStar::makeControl() {
  control_box.time_stamp = local_clock + 1;
  control_box.accu = count;
  control_box.invalid = 0;
  control_box.init = rank;
  control_box.result = result;
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
  if (!flag)
    return 0;

  MPI_Recv(&control_box, 1, control_struct, MPI_ANY_SOURCE, CONTROL_TAG, MPI_COMM_WORLD, &status);
  local_clock = time(NULL);
  if (local_clock < control_box.time_stamp)
    local_clock = control_box.time_stamp;

  if (control_box.init == rank) {
    if (control_box.accu == 0 && !control_box.invalid)
      is_end = 1;
  } else {
    control_box.accu += count;
    control_box.invalid = control_box.invalid || tmax >= control_box.time_stamp
                        || !checkResult(result);
    MPI_Bsend(&control_box, 1, control_struct, (rank+1) % node_size, CONTROL_TAG, MPI_COMM_WORLD);
  }

  return 1;
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

    if (!updateMin())
      continue;

    expand(open[min].back());

    if (result != 255) {
      makeControl();
      MPI_Bsend(&control_box, 1, control_struct, (rank+1) % node_size, CONTROL_TAG, MPI_COMM_WORLD);
    }
  }

  return -1;
}

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);
  // uint8_t tiles[] = {0, 5, 2, 3, 1, 4, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  uint8_t tiles[] = {0, 1, 9, 7, 11, 13, 5, 3, 14, 12, 4, 2, 8, 6, 10, 15};
  // int8_t tiles[] = {0, 13, 2, 4, 12, 14, 6, 9, 15, 1, 10, 3, 11, 5, 8, 7};
  // int8_t tiles[] = {15, 10, 8, 3, 0, 6, 9, 5, 1, 14, 13, 11, 7, 2, 12, 4};
  // int8_t tiles[] = {7, 6, 8, 1, 11, 5, 14, 10, 3, 4, 9, 13, 15, 2, 0, 12};
  HDAStar solver;
  cout << solver.solve(tiles, 0) << endl;
  // cout << solver.solve(tiles, 4) << endl;
  // cout << solver.solve(tiles, 14) << endl;
  MPI_Finalize();
  exit(0);
}
