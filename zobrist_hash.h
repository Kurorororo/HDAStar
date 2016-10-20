class ZobristHash {
private:
  int field_size;
  int characters;
public:
  std::vector< std::vector<uint64_t> > table;
  ZobristHash(int, int);
  void calc_zobrist_hash();
  void n_length_randoms(int);
};
