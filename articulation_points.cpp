#include "articulation_points.h"
using std::min, std::max, std::set, std::unordered_map, std::vector, std::pair, std::cout;
vector<pair<int, int>> construct_largest_neighbor_edges(vector<pair<int, int>>& edges){
  unordered_map<int, int> vertex_degree_mapping;
  for(int i =0 ; i < edges.size(); i++){
    vertex_degree_mapping[edges.at(i).first]++;
    vertex_degree_mapping[edges.at(i).second]++;
  }
  
  unordered_map<int, pair<int, int>> largest_neighbor_mapping;
  for(int i = 0; i < edges.size(); i++){
    int vertex_one = edges.at(i).first;
    int vertex_two = edges.at(i).second;
    int vertex_one_size = vertex_degree_mapping[vertex_one];
    int vertex_two_size = vertex_degree_mapping[vertex_two];
    
    int vertex_one_curr_largest_neighbor_size = 0;
    int vertex_two_curr_largest_neighbor_size = 0;
    if(largest_neighbor_mapping.count(vertex_one) != 0){
      vertex_one_curr_largest_neighbor_size = largest_neighbor_mapping[vertex_one].second;
    }
    if(largest_neighbor_mapping.count(vertex_two) != 0){
      vertex_two_curr_largest_neighbor_size = largest_neighbor_mapping[vertex_two].second;
    }
    if(vertex_one_size > vertex_two_curr_largest_neighbor_size){
      largest_neighbor_mapping[vertex_two] = pair<int, int>(vertex_one, vertex_one_size);
    }
    if(vertex_two_size > vertex_one_curr_largest_neighbor_size){
      largest_neighbor_mapping[vertex_one] = pair<int, int>(vertex_two, vertex_two_size);
    }
  }
  vector<pair<int, int>> result;
  set<pair<int, int>> already_accounted;
  for(auto it = largest_neighbor_mapping.begin(); it != largest_neighbor_mapping.end(); it++){
    int start_vertex = min(it->first, it->second.first);
    int end_vertex = max(it->first, it->second.first);
    pair<int, int> edge_pair(start_vertex, end_vertex);
    if(already_accounted.count(edge_pair) != 0){
      continue;
    }
    result.push_back(edge_pair);
    already_accounted.emplace(edge_pair);
  }
  return result;
}
vector<int> articulation_points(vector<pair<int, int>>& edges){
  /*
  Assumes that input edges are undirected, and does not contain duplicate edges.
  main idea: create spanning trees, vertices in spanning trees with degree 1 are not articulation points
   */
  vector<int> result;
  vector<pair<int, int>> known_edges;
  vector<pair<int, int>> unknown_edges;
  vector<pair<int, int>> largest_neighbor_edges = construct_largest_neighbor_edges(edges);
  return result; 

}
