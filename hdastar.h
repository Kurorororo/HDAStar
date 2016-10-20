#include "state.cpp"

struct Message {
  uint32_t time_stamp;
  State state;
};

struct ControlMessage {
  int32_t time_stamp;
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

  int updateMin();
  void expand(State*);
  void checkKid(State*);
  void createMessageStruct();
  void createControlStruct();
  void createBuffer();
  void sendMessage(int, State*);
  void recieveMessage();
  void makeControl();
  int checkResult(uint8_t);
  int recieveControl();

  std::vector<State*> open[255];
  std::unordered_set<uint64_t> closed;
  uint8_t min;
  uint8_t result;
  int node_size;
  int rank;
  vector< vector<uint64_t> > zobrist_table;
  vector<Message> message_box;
  ControlMessage control_box;
  MPI_Datatype state_struct, message_struct, control_struct;
  Message *buff;
  int count;
  int32_t tmax;
  int32_t local_clock;
  int is_end;
};
