test: 
	g++ -std=c++17 test.cpp uf.cpp bridges.cpp articulation_points.cpp -lgtest -lgtest_main -pthread -o test
	./test

artic:
	g++ -std=c++17 test.cpp uf.cpp bridges.cpp articulation_points.cpp -lgtest -lgtest_main -pthread -o test
	./test --gtest_filter=ArticulationPointsTest.*

parallel:
	g++ -std=c++20 parallel_runner.cpp bridges.cpp -o parallel_runner

