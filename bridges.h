#ifndef HEADERFILE_H
#define HEADERFILE_H
#include <bits/stdc++.h>
#include <ygm/comm.hpp>
#include <ygm/comm.hpp>
#include <ygm/container/map.hpp>
#include <ygm/container/set.hpp>
#include <ygm/container/bag.hpp>
#include <ygm/container/disjoint_set.hpp>
#include <ygm/io/csv_parser.hpp>
#include <ygm/collective.hpp>
#include <chrono>
#include "uf.h"
using std::pair, std::vector, std::unordered_map, std::min, std::string;
vector<pair<long long, long long>> find_bridges(vector<pair<long long, long long>>& edges, size_t num_vertices);
vector<pair<long long, long long>> find_bridges_tarjan(vector<pair<long long, long long>>& edges);
double find_bridges_parallel(string& filename, ygm::comm& world);
void find_bridges_parallel_opt(string& filename, ygm::comm& world);
void test_disjoint_set(string& csv_file, ygm::comm& world);
#endif
