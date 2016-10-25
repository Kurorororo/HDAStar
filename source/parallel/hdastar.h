#include "state.h"

struct Message {
  int time_stamp;
  State state;
};

struct ControlMessage {
  int time_stamp;
  int accu;
  int invalid;
  int init;
  uint8_t result;
};

class HDAStar {
 public:
  HDAStar();
  int solve(uint8_t[], uint8_t);

 private:
  static const int MESSAGE_TAG = 0;
  static const int CONTROL_TAG = 1;
  static const int TERMINATE_TAG = 2;

  int updateMin();
  void expand(State*);
  int isClosed(const uint64_t&, uint8_t);
  void checkKid(State*);
  void createBuffer();
  void sendMessage(int, State*);
  void recieveMessage();
  void sendControl();
  int isResult(uint8_t);
  int recieveControl();
  void sendTerminate();
  int recieveTerminate();

  std::vector<State*> open[255];
  std::unordered_map<uint64_t, std::pair<uint8_t, uint64_t> > closed;
  uint8_t min;
  uint8_t result;
  int is_end;
  int node_size;
  int rank;
  Message message_box;
  ControlMessage control_box;
  Message *buff;
  int count;
  int32_t tmax;
  int local_clock;
  int discoverd;
};
