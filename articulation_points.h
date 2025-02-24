#include <bits/stdc++.h>
using std::vector, std::pair;
struct Edge_With_Count{
  int vertex_one;
  int vertex_two;
  int count;
};
vector<int> articulation_points(vector<pair<int, int>>& edges, size_t num_vertices);
