#include "uf.h"
using std::vector;
Union_Find::Union_Find(){}
Union_Find::Union_Find(size_t size){
	nodes = vector<Node>(size);
  for(size_t i = 0; i < nodes.size(); i++){
    nodes.at(i).parent = i;
  }
}
bool Union_Find::merge(size_t index_one, size_t index_two){
	size_t parent_one = find(index_one);
  size_t parent_two = find(index_two);
  if(parent_one == parent_two){
    return false;
  }
  //parent one is larger tree, so merge parent two into parent one
  if(nodes[parent_one].rank > nodes[parent_two].rank){
    nodes[parent_two].parent = parent_one;
  }
  else{  
    nodes[parent_one].parent = parent_two;
    if(nodes[parent_one].rank == nodes[parent_two].rank){
      nodes[parent_two].rank++;
    }
  }
  return true;

}
size_t Union_Find::find(size_t index){
  size_t parent = nodes.at(index).parent;
  if(nodes.at(parent).parent != parent){
    size_t real_parent = find(parent);
    nodes[index].parent = real_parent;
    return real_parent;
  }  
  return parent;
}
