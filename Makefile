test: 
	g++ -std=c++17 test.cpp uf.cpp bridges.cpp -lgtest -lgtest_main -pthread -o test
	./test

