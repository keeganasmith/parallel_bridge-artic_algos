#include <bits/stdc++.h>
#include <ygm/comm.hpp>
#include <ygm/comm.hpp>
#include <ygm/container/map.hpp>
#include <ygm/container/set.hpp>
#include <ygm/container/bag.hpp>
#include <ygm/io/csv_parser.hpp>
#include "uf.h"
using std::pair, std::vector, std::unordered_map, std::min, std::string;
vector<pair<int, int>> find_bridges(vector<pair<int, int>>& edges, size_t num_vertices);
vector<pair<int, int>> find_bridges_tarjan(vector<pair<int, int>>& edges);
void find_bridges_parallel(string& filename, ygm::comm& world);
