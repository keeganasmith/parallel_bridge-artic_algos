#include <bits/stdc++.h>
#include "uf.h"
using std::pair, std::vector, std::unordered_map, std::min;
vector<pair<int, int>> find_bridges(vector<pair<int, int>>& edges, size_t num_vertices);
vector<pair<int, int>> find_bridges_tarjan(vector<pair<int, int>>& edges);
