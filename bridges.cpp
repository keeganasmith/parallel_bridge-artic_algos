#include "bridges.h"
using std::vector, std::pair;
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

