#include "uf.h"
#include "articulation_points.h"
using std::sort, std::unordered_set, std::min, std::max, std::queue, std::set, std::unordered_map, std::vector, std::pair, std::cout;
vector<Edge_With_Count> construct_largest_neighbor_edges(vector<pair<int, int>>& edges){
  unordered_map<int, int> vertex_degree_mapping;
  for(int i =0 ; i < edges.size(); i++){
    vertex_degree_mapping[edges.at(i).first]++;
    vertex_degree_mapping[edges.at(i).second]++;
  }
  
  unordered_map<int, vector<pair<int, int>>> largest_neighbor_mapping;
  for(int i = 0; i < edges.size(); i++){
    int vertex_one = edges.at(i).first;
    int vertex_two = edges.at(i).second;
    int vertex_one_size = vertex_degree_mapping[vertex_one];
    int vertex_two_size = vertex_degree_mapping[vertex_two];
    
    int vertex_one_curr_largest_neighbor_size = 0;
    int vertex_two_curr_largest_neighbor_size = 0;
    if(largest_neighbor_mapping.count(vertex_one) != 0){
      vertex_one_curr_largest_neighbor_size = largest_neighbor_mapping[vertex_one].back().second;
    }
    if(largest_neighbor_mapping.count(vertex_two) != 0){
      vertex_two_curr_largest_neighbor_size = largest_neighbor_mapping[vertex_two].back().second;
    }
    if(vertex_one_size > vertex_two_curr_largest_neighbor_size){
      largest_neighbor_mapping[vertex_two] = vector<pair<int, int>>();
      largest_neighbor_mapping[vertex_two].push_back(pair<int, int>(vertex_one, vertex_one_size));
    }
    else if(vertex_one_size == vertex_two_curr_largest_neighbor_size){
      largest_neighbor_mapping[vertex_two].push_back(pair<int, int>(vertex_one, vertex_one_size));
    }
    if(vertex_two_size > vertex_one_curr_largest_neighbor_size){
      largest_neighbor_mapping[vertex_one] = vector<pair<int, int>>();
      largest_neighbor_mapping[vertex_one].push_back(pair<int, int>(vertex_two, vertex_two_size));
    }
    else if(vertex_two_size == vertex_one_curr_largest_neighbor_size){
      largest_neighbor_mapping[vertex_one].push_back(pair<int, int>(vertex_two, vertex_two_size));
    }
  }
  vector<Edge_With_Count> result;
  set<pair<int, int>> already_accounted;
  for(auto it = largest_neighbor_mapping.begin(); it != largest_neighbor_mapping.end(); it++){
    for(int i = 0; i < it->second.size(); i++){
      int start_vertex = min(it->first, it->second.at(i).first);
      int end_vertex = max(it->first, it->second.at(i).first);
      Edge_With_Count curr_edge;
      curr_edge.vertex_one = start_vertex;
      curr_edge.vertex_two = end_vertex;
      curr_edge.count = 0;
      pair<int, int> edge_pair(start_vertex, end_vertex);
      if(already_accounted.count(edge_pair) != 0){
        continue;
      }
      result.push_back(curr_edge);
      already_accounted.emplace(edge_pair);
    }
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
  Union_Find marked_vertices_union(num_vertices);
  unordered_set<int> marked_vertices;
  vector<int> result;
  while(marked_vertices.size() != num_vertices){
    Union_Find spanning_tree(num_vertices);
    unordered_map<int, int> vertex_degree;
    //iterate through edges, only add edges which do not contain a marked vertex
    for(int i = 0; i < edges.size(); i++){
      //if not in the marked vertices yet, we need to keep track of its degree in the spanning tree
      if(marked_vertices.count(edges.at(i).first) == 0){
        vertex_degree[edges.at(i).first] = 0;
      }
      if(marked_vertices.count(edges.at(i).second) == 0){
        vertex_degree[edges.at(i).second] = 0;
      }
      //both vertices must not be in marked
      if((marked_vertices.count(edges.at(i).first) == 0) && (marked_vertices.count(edges.at(i).second) == 0)){
        bool merged = spanning_tree.merge(edges.at(i).first, edges.at(i).second);
        if(merged){
          vertex_degree[edges.at(i).first]++;
          vertex_degree[edges.at(i).second]++;
        }
      }
    }

    //find and mark all vertices of degree 1 or less
    unordered_set<int> newly_marked_vertices;
    for(auto it = vertex_degree.begin(); it != vertex_degree.end(); it++){
      if(it->second <= 1){
        newly_marked_vertices.emplace(it->first);
        marked_vertices.emplace(it->first);
      }
    }

    unordered_map<int, int> num_times_merged;
    for(int i = 0; i < edges.size(); i++){
      int first_vertex = edges.at(i).first;
      int second_vertex = edges.at(i).second;
      //if both vertices are marked, and at least one is newly marked
      if((marked_vertices.count(first_vertex) != 0) && (marked_vertices.count(second_vertex) != 0)){
        bool first_point_is_new = (newly_marked_vertices.count(first_vertex) != 0);
        bool second_point_is_new = (newly_marked_vertices.count(second_vertex) != 0);
        if(first_point_is_new && second_point_is_new){
          bool merged = marked_vertices_union.merge(first_vertex, second_vertex);
          if(merged){
            num_times_merged[first_vertex]++;
            num_times_merged[second_vertex]++;
          }
        }
        else if(first_point_is_new){
          bool merged = marked_vertices_union.merge(first_vertex, second_vertex);
          if(merged){
            num_times_merged[first_vertex]++;
          }
        } 
        else if(second_point_is_new){
          bool merged = marked_vertices_union.merge(first_vertex, second_vertex);
          if(merged){
            num_times_merged[second_vertex]++;
          }
        }
      }
    }
    for(auto it = num_times_merged.begin(); it != num_times_merged.end(); it++){
      if(it->second > 1){
        result.push_back(it->first);
      }
    }
  }
    
  return result; 
}
