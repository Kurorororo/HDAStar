hdastar: source/parallel/hdastar.cpp source/parallel/state.cpp source/parallel/zobrist_hash.cpp source/common/manhattan_dist.cpp
	mpic++ -Wall source/parallel/hdastar.cpp source/parallel/state.cpp source/parallel/zobrist_hash.cpp source/common/manhattan_dist.cpp -o hdastar -lboost_system -std=c++11
astar: source/single/astar.cpp source/single/state.cpp source/common/manhattan_dist.cpp
	g++ -Wall source/single/astar.cpp source/single/state.cpp source/common/manhattan_dist.cpp -o astar -lboost_system -std=c++11
clean:
	rm -f *.o hdastar astar
