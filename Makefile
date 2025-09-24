CC := mpicc
CXX := mpicxx
FC := mpif90
CLUSTER := aces
CFLAGS := -O3 
ifeq ($(CLUSTER),ookami)
CFLAGS := -O3 -mcpu=a64fx
endif
CXXFLAGS := $(CFLAGS)
FCFLAGS := $(CFLAGS)
test: 
	mpic++ -std=c++20 $(CXXFLAGS) -fcommon -Wl,--allow-multiple-definition test.cpp uf.cpp bridges.cpp articulation_points.cpp -lgtest -lgtest_main -pthread -o test
	./test

artic:
	g++ -std=c++17 test.cpp uf.cpp bridges.cpp articulation_points.cpp -lgtest -lgtest_main -pthread -o test
	./test --gtest_filter=ArticulationPointsTest.*

parallel:
	rm -f *.o test parallel_runner
	mpic++ -std=c++20 $(CXXFLAGS) -fcommon -Wl,--allow-multiple-definition parallel_runner.cpp bridges.cpp uf.cpp -o parallel_runner

clean:
	rm -f *.o test parallel_runner

generate:
	rm -f *.o test parallel_runner generate
	mpic++ -std=c++20 -fcommon -Wl,--allow-multiple-definition generate_random_graph.cpp -o generate
	
verify:
	mpic++ -std=c++20 -fcommon -Wl,--allow-multiple-definition -fopenmp verify_bridges.cpp bridges.cpp uf.cpp -o verify 
