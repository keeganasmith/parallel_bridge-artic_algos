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
  vector<int> result;
  set<pair<int, int>> known_edges;
  set<pair<int, int>> almost_known_edges; //edges connected to neighbor of known
  vector<pair<int, int>> unknown_edges(edges);
  vector<Edge_With_Count> largest_neighbor_edges = construct_largest_neighbor_edges(edges);
  unordered_set<int> known_vertices;
  unordered_set<int> vertices_connected_to_known;
  while(true){
    Union_Find uf(num_vertices);
    unordered_map<int, int> vertex_degree_spanning;
    for(auto it = known_edges.begin(); it != known_edges.end(); it++){
      bool merged = uf.merge(it->first, it->second);
      if(merged){
        cout << "known connected " << it->first << ", " << it->second << "\n";
        vertex_degree_spanning[it->first]++;
        vertex_degree_spanning[it->second]++;
      }
    }
    
    vector<int> edges_connecting_two_unknowns_largest_indices;
    vector<int> edges_connected_to_neighbor_of_known_largest_indices;
    //only add edges where one of the vertices is connected to a known vertex
    for(int i = 0; i < largest_neighbor_edges.size(); i++){
      int vertex_one = largest_neighbor_edges.at(i).vertex_one;
      int vertex_two = largest_neighbor_edges.at(i).vertex_two;
      if(known_vertices.count(vertex_one) == 0 && known_vertices.count(vertex_two) == 0){
        if(vertices_connected_to_known.count(vertex_one) != 0 || vertices_connected_to_known.count(vertex_two) != 0){
          edges_connected_to_neighbor_of_known_largest_indices.push_back(i);
        }
        else{
          edges_connecting_two_unknowns_largest_indices.push_back(i);
        }
        continue;
      }
      bool merged = uf.merge(vertex_one, vertex_two);
      if(merged){
        cout << "largest neighbor connected " << vertex_one << ", " << vertex_two << "\n";
        vertex_degree_spanning[vertex_one]++;
        vertex_degree_spanning[vertex_two]++;
        largest_neighbor_edges.at(i).count++;
      }
    }

    //only add unknown edges where one of the vertices is connected to a known
    //vertex
    vector<pair<int, int>> edges_connecting_two_unknowns;
    vector<pair<int, int>> edges_connected_to_neighbor_of_known;
    for(int i = 0; i < unknown_edges.size(); i++){
      if(known_vertices.count(unknown_edges.at(i).first) == 0 && known_vertices.count(unknown_edges.at(i).second) == 0){
        if(vertices_connected_to_known.count(unknown_edges.at(i).first) != 0 || vertices_connected_to_known.count(unknown_edges.at(i).second) != 0){
          edges_connected_to_neighbor_of_known.push_back(unknown_edges.at(i));
        }
        else{
          edges_connecting_two_unknowns.push_back(unknown_edges.at(i));
        }
        continue;
      }
      bool merged = uf.merge(unknown_edges.at(i).first, unknown_edges.at(i).second);
      if(merged){
        cout << "known + unknown connected " << unknown_edges.at(i).first << ", " << unknown_edges.at(i).second << "\n";
        vertex_degree_spanning[unknown_edges.at(i).first]++;
        vertex_degree_spanning[unknown_edges.at(i).second]++;
      }
    }  
    
    //only add edges where both of the vertices are connected to a known vertex
    for(auto it = almost_known_edges.begin(); it != almost_known_edges.end(); it++){
      int first = it->first;
      int second = it->second;
      bool merged = uf.merge(first, second);
      if(merged){
        cout << "neighbor of + neighbor of connected " << first << ", " << second << "\n";
        vertex_degree_spanning[first]++;
        vertex_degree_spanning[second]++;
      }    
    }
    //connect vertices which are neighbors to known first for largest
    for(int i = 0;i < edges_connected_to_neighbor_of_known_largest_indices.size(); i++){
      int index = edges_connected_to_neighbor_of_known_largest_indices.at(i);
      int vertex_one = largest_neighbor_edges.at(index).vertex_one;
      int vertex_two = largest_neighbor_edges.at(index).vertex_two;
      bool merged = uf.merge(vertex_one, vertex_two);
      if(merged){
        cout << "unknown edges largest, known neighbor(s), connected " << vertex_one << ", " << vertex_two << "\n";
        vertex_degree_spanning[vertex_one]++;
        vertex_degree_spanning[vertex_two]++;
        largest_neighbor_edges.at(index).count++;
      }
    } 
    
    for(int i = 0;i < edges_connecting_two_unknowns_largest_indices.size(); i++){
      int index = edges_connecting_two_unknowns_largest_indices.at(i);
      int vertex_one = largest_neighbor_edges.at(index).vertex_one;
      int vertex_two = largest_neighbor_edges.at(index).vertex_two;
      bool merged = uf.merge(vertex_one, vertex_two);
      if(merged){
        cout << "unknown edges largest connected " << vertex_one << ", " << vertex_two << "\n";
        vertex_degree_spanning[vertex_one]++;
        vertex_degree_spanning[vertex_two]++;
        largest_neighbor_edges.at(index).count++;
      }
    }

    for(int i = 0;i < edges_connecting_two_unknowns.size(); i++){
      bool merged = uf.merge(edges_connecting_two_unknowns.at(i).first, edges_connecting_two_unknowns.at(i).second);
      if(merged){
        cout << "unknown edges connected " << edges_connecting_two_unknowns.at(i).first << ", " << edges_connecting_two_unknowns.at(i).second << "\n";
        vertex_degree_spanning[edges_connecting_two_unknowns.at(i).first]++;
        vertex_degree_spanning[edges_connecting_two_unknowns.at(i).second]++;
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
      else if(vertices_connected_to_known.count(first_vertex) != 0 && vertices_connected_to_known.count(second_vertex)){
        almost_known_edges.emplace(pair<int, int>(first_vertex, second_vertex));
      }
      //if one is known, add unkown vertex to known neighbors
      if(known_vertices.count(first_vertex) != 0){
        vertices_connected_to_known.emplace(second_vertex);
      }
      if(known_vertices.count(second_vertex) != 0){
        vertices_connected_to_known.emplace(first_vertex);
      }
    }

    sort(largest_neighbor_edges.begin(), largest_neighbor_edges.end(),
          [](const Edge_With_Count& a, const Edge_With_Count& b) {
              return a.count < b.count;
          });
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
