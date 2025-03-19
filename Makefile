test: 
	g++ -std=c++17 test.cpp uf.cpp bridges.cpp articulation_points.cpp -lgtest -lgtest_main -pthread -o test
	./test

artic:
	g++ -std=c++17 test.cpp uf.cpp bridges.cpp articulation_points.cpp -lgtest -lgtest_main -pthread -o test
	./test --gtest_filter=ArticulationPointsTest.*

parallel:
	mpic++ -std=c++20 -fcommon -Wl,--allow-multiple-definition parallel_runner.cpp bridges.cpp uf.cpp -o parallel_runner

