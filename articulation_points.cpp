#include "uf.h"
#include "articulation_points.h"
using std::unordered_set, std::min, std::max, std::set, std::unordered_map, std::vector, std::pair, std::cout;
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
vector<int> articulation_points(vector<pair<int, int>>& edges, size_t num_vertices){
  /*
  Assumes that input edges are undirected, and does not contain duplicate edges.
  main idea: create spanning trees, vertices in spanning trees with degree 1 are not articulation points
  attempt to ensure progress by adding edges which connect known vertices first. Increase speed of progress 
  by adding edges to largest neighbor first before randomly adding edges to spanning tree.
   */
  vector<int> result;
  set<pair<int, int>> known_edges;
  vector<pair<int, int>> unknown_edges(edges);
  vector<pair<int, int>> largest_neighbor_edges = construct_largest_neighbor_edges(edges);
  unordered_set<int> known_vertices;
  while(true){
    Union_Find uf(num_vertices);
    unordered_map<int, int> vertex_degree_spanning;
    for(auto it = known_edges.begin(); it != known_edges.end(); it++){
      bool merged = uf.merge(it->first, it->second);
      if(merged){
        vertex_degree_spanning[it->first]++;
        vertex_degree_spanning[it->second]++;
      }
    }

    for(int i = 0; i < largest_neighbor_edges.size(); i++){
      bool merged = uf.merge(largest_neighbor_edges.at(i).first, largest_neighbor_edges.at(i).second);
      if(merged){
        vertex_degree_spanning[largest_neighbor_edges.at(i).first]++;
        vertex_degree_spanning[largest_neighbor_edges.at(i).second]++;
      }
    }
    for(int i = 0; i < unknown_edges.size(); i++){
      bool merged = uf.merge(unknown_edges.at(i).first, unknown_edges.at(i).second);
      if(merged){
        vertex_degree_spanning[unknown_edges.at(i).first]++;
        vertex_degree_spanning[unknown_edges.at(i).second]++;
      }
    }  
    bool found_new_vertex = false;
    //find vertices which have degree <= 1
    int new_vertices_found = 0;
    for(auto it = vertex_degree_spanning.begin(); it != vertex_degree_spanning.end(); it++){
      if((it->second <= 1) && (known_vertices.count(it->first) == 0)){
        known_vertices.emplace(it->first);
        found_new_vertex = true;
        new_vertices_found++;
      }
    }
    cout << "found " << new_vertices_found << " new vertices\n";
    if(!found_new_vertex){
      break;
    }

    for(int i = 0; i < unknown_edges.size(); i++){
      int first_vertex = unknown_edges.at(i).first;
      int second_vertex = unknown_edges.at(i).second;
      //if both are known, add edge to known edges
      if((known_vertices.count(first_vertex) != 0) && (known_vertices.count(second_vertex) != 0)){
        known_edges.emplace(pair<int, int>(first_vertex, second_vertex));
      }
    }
  }
  
  unordered_set<int> already_placed;
  for(int i =0; i < edges.size(); i++){
    int vertex_one = edges.at(i).first;
    int vertex_two = edges.at(i).second;
    if((known_vertices.count(vertex_one) == 0) && (already_placed.count(vertex_one) == 0)){
      already_placed.emplace(vertex_one);
      result.push_back(vertex_one);
    }
    if((known_vertices.count(vertex_two) == 0) && (already_placed.count(vertex_two) == 0)){
      already_placed.emplace(vertex_two);
      result.push_back(vertex_two);
    }
  
  }

  return result; 

}
