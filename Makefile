hdastar: parallel/hdastar.cpp parallel/state.cpp
	mpic++ -Wall parallel/hdastar.cpp parallel/state.cpp -o hdastar -lboost_system -std=c++11
astar: single/astar.cpp single/state.cpp
	g++ -Wall single/astar.cpp single/state.cpp -o astar -lboost_system -std=c++11
manhattan_dist: common/manhattan_dist.cpp
	g++ -Wall common/manhattan_dist.cpp -o manhattan_dist
zobrist_hash: common/zobrist_hash.cpp
	g++ -Wall common/zobrist_hash.cpp -o zobrist_hash
clean:
	rm -f *.o hdastar manhattan_dist zobrist_hash
