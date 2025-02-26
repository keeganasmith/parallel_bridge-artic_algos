#include "bridges.h"
using std::cout, std::vector, std::pair;
vector<pair<int, int>> find_bridges(vector<pair<int, int>>& edges, size_t num_vertices){
  vector<pair<int, int>> not_bridges;
  vector<pair<int, int>> maybe_bridges(edges);
  while(true){
    Union_Find uf(num_vertices);
    vector<pair<int, int>> new_maybe_bridges;
    for(size_t i = 0; i < not_bridges.size(); i++){
      uf.merge(not_bridges.at(i).first, not_bridges.at(i).second);
    } 
    for(size_t i = 0; i < maybe_bridges.size(); i++){
      bool merged = uf.merge(maybe_bridges.at(i).first, maybe_bridges.at(i).second);
      if(!merged){
        not_bridges.push_back(maybe_bridges.at(i));
      }
      else{
        new_maybe_bridges.push_back(maybe_bridges.at(i));
      }
    }
    if(new_maybe_bridges.size() == maybe_bridges.size()){
      break;
    }
    maybe_bridges = new_maybe_bridges;
  }
  return maybe_bridges;
}

void dfs(unordered_map<int, vector<int>>& graph, int vertex, int parent, int& time,
         unordered_map<int, int>& discovery_times, unordered_map<int, int>& low_times, 
         vector<pair<int, int>>& result) {
    
  discovery_times[vertex] = low_times[vertex] = ++time;

  for (int child : graph[vertex]) {
    if (child == parent) continue;

    if (discovery_times.find(child) == discovery_times.end()) {
      dfs(graph, child, vertex, time, discovery_times, low_times, result);

      low_times[vertex] = min(low_times[vertex], low_times[child]);

      if (low_times[child] > discovery_times[vertex]) {
        result.push_back({vertex, child});
      }
    } else {
      low_times[vertex] = min(low_times[vertex], discovery_times[child]);
    }
  }
}

vector<pair<int, int>> find_bridges_tarjan(vector<pair<int, int>>& edges) {
  unordered_map<int, vector<int>> graph;
  for (const auto& edge : edges) {
    graph[edge.first].push_back(edge.second);
    graph[edge.second].push_back(edge.first);
  }

  vector<pair<int, int>> result;
  int time = 0;
  unordered_map<int, int> discovery_times, low_times;

  for (const auto& it : graph) {
    if (discovery_times.find(it.first) == discovery_times.end()) {
      dfs(graph, it.first, -1, time, discovery_times, low_times, result);
    }
  }
  return result;
}
