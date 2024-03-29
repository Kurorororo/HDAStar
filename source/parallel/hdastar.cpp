#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include <mpi.h>
#include "hdastar.h"
using namespace std;

HDAStar::HDAStar() {
  min = 0;
  result = 255;
  is_end = 0;
  count = 0;
  tmax = 0;
  local_clock = 0;
  discovered = 0;
  for (int i=0; i<255; ++i)
    open[i].reserve(10000);
  closed.reserve(100003);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &node_size);

  message_box = vector< vector<Message> >(node_size, vector<Message>(MAX_MESSAGES));
  box_index = vector<int>(node_size, 0);

  createBuffer();
}

void HDAStar::createBuffer() {
  int b_size = sizeof(message_box[0][0]) * MAX_MESSAGES * node_size;
  b_size += MPI_BSEND_OVERHEAD * node_size;
  if ((buff = (Message*)malloc(b_size)) == NULL) {
    cout << "\nInsufficient memory for Bsend buffer" << endl;
    fflush(NULL);
    exit(1);
  }
  MPI_Buffer_attach(buff, b_size);
}

int HDAStar::updateMin() {
  if (min >= result)
    return 0;

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
  open[min].pop_back();
  closed[s->tiles] = make_pair(s->g, s->parent);
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

int HDAStar::isClosed(const uint64_t &tiles, uint8_t g) {
  if (closed.find(tiles) == closed.end())
    return 0;
  if (closed[tiles].first <= g)
    return 1;
  return 0;
}

void HDAStar::checkKid(State *kid) {
  if (isClosed(kid->tiles, kid->g))
    return;

  if (kid->isGoal()) {
    discovered = 1;
    result = kid->g;
    min = kid->g;
    return;
  }

  int dest = kid->hash % node_size;
  if (dest == rank) {
    if (kid->f() < min)
      min = kid->f();
    open[kid->f()].push_back(kid);
    return;
  }

  pushMessage(dest, kid);
}

void HDAStar::pushMessage(int dest, State *s) {
  message_box[dest][box_index[dest]].time_stamp = local_clock;
  message_box[dest][box_index[dest]].state = *s;
  ++box_index[dest];
  if (box_index[dest] == MAX_MESSAGES) {
    sendMessage(dest);
  }
  closed[s->tiles] = make_pair(s->g, s->parent);
  State::last(s);
}

void HDAStar::sendMessage(int dest) {
  MPI_Bsend(&message_box[dest][0], sizeof(message_box[dest][0])*box_index[dest], MPI_BYTE, dest, MESSAGE_TAG, MPI_COMM_WORLD);
  ++count;
  box_index[dest] = 0;
}

void HDAStar::sendAllMessages() {
  for (int i=0; i<node_size; ++i)
    if (box_index[i] > 0)
      sendMessage(i);
}

void HDAStar::recieveMessage() {
  int flag;
  MPI_Status status;
  MPI_Iprobe(MPI_ANY_SOURCE, MESSAGE_TAG, MPI_COMM_WORLD, &flag, &status);
  while (flag) {
    int message_count;
    MPI_Get_count(&status, MPI_BYTE, &message_count);
    MPI_Recv(&message_box[rank][0], message_count, MPI_BYTE, MPI_ANY_SOURCE, MESSAGE_TAG, MPI_COMM_WORLD, &status);
    int last_index = message_count/sizeof(message_box[rank][0]) - 1;
    if (tmax < message_box[rank][last_index].time_stamp)
      tmax = message_box[rank][last_index].time_stamp;

    for (int i=0; i<=last_index; ++i) {
      State *s = State::clone(message_box[rank][i].state);
      if (isClosed(s->tiles, s->g))
        continue;
      if (s->f() < min)
        min = s->f();
      open[s->f()].push_back(s);
    }
    --count;
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

int HDAStar::isResult(uint8_t result) {
  for (uint8_t i=0; i<result; ++i) {
    if (!(open[i].empty()))
      return 0;
  }
  return 1;
}

int HDAStar::recieveControl() {
  int flag;
  MPI_Status status;
  MPI_Iprobe(MPI_ANY_SOURCE, CONTROL_TAG, MPI_COMM_WORLD, &flag, &status);
  if (!flag)
    return 0;

  MPI_Recv(&control_box, sizeof(control_box), MPI_BYTE, MPI_ANY_SOURCE,CONTROL_TAG, MPI_COMM_WORLD, &status);
  if (local_clock < control_box.time_stamp)
    local_clock = control_box.time_stamp;

  if (control_box.init == rank) {
    if (control_box.accu == 0 && !control_box.invalid) {
      sendTerminate();
      return 1;
    }
  } else {
    if (result > control_box.result)
      result = control_box.result;
    control_box.accu += count;
    control_box.invalid = control_box.invalid || tmax >= control_box.time_stamp || !isResult(control_box.result);
    MPI_Bsend(&control_box, sizeof(control_box), MPI_BYTE, (rank+1) %node_size, CONTROL_TAG, MPI_COMM_WORLD);
  }
  return 0;
}

void HDAStar::sendTerminate() {
  int a = 1;
  for (int i=0; i<node_size; ++i) {
    if (i != rank)
      MPI_Bsend(&a, 1, MPI_INT, i, TERMINATE_TAG, MPI_COMM_WORLD);
  }
}

int HDAStar::recieveTerminate() {
  int flag;
  MPI_Status status;
  MPI_Iprobe(MPI_ANY_SOURCE, TERMINATE_TAG, MPI_COMM_WORLD, &flag, &status);
  if (flag)
    return 1;
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
      pushMessage(dest, s);
    }
  }
  while (true) {
    if (recieveTerminate() || recieveControl()) {
      return result;
    }
    if (discovered) {
      sendControl();
    }
    recieveMessage();
    if (!updateMin()) {
      sendAllMessages();
      continue;
    }
    expand(open[min].back());
  }
  return -1;
}

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);
  int blank;
  cin >> blank;
  int in_tile;
  uint8_t tiles[16];
  for (int i=0; i<16; ++i) {
    cin >> in_tile;
    tiles[i] = in_tile;
  }
  HDAStar solver;
  int result = solver.solve(tiles, static_cast<uint8_t>(blank));
  cout << result << endl;
  MPI_Finalize();
  exit(0);
}
