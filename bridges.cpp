#include "bridges.h"
using std::cout, std::vector, std::pair, std::endl;
vector<pair<long long, long long>> find_bridges(vector<pair<long long, long long>>& edges, size_t num_vertices){
  vector<pair<long long, long long>> not_bridges;
  vector<pair<long long, long long>> maybe_bridges(edges);
  while(true){
    Union_Find uf(num_vertices);
    vector<pair<long long, long long>> new_maybe_bridges;
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

void find_bridges_parallel(string& csv_file, ygm::comm& world){
  //csv file must be in the format:
  //<vertex one>,<vertex two>
  //<vertex one>,<vertex two>
  //...
  //<vertex one>,<vertex two>
  //and should not contain duplicates
  static ygm::comm* s_world = &world;
  vector<string> filenames(1, csv_file);
  ygm::io::csv_parser parser(world, filenames);
  static vector<pair<long long, long long>> not_bridges;
  static vector<pair<long long, long long>> maybe_bridges;
  parser.for_all([](ygm::io::detail::csv_line line){
    long long vertex_one = line[0].as_integer();
    long long vertex_two = line[1].as_integer();
    maybe_bridges.push_back(pair<long long, long long>(vertex_one, vertex_two)); 
  });
  int num_iterations = 0;
  world.barrier();
  while(true){
    static ygm::container::disjoint_set<long long> disjoint(world); //use async_union_and_execute
    static vector<pair<long long, long long>> new_maybe_bridges;
    const auto start{std::chrono::steady_clock::now()};
    for(size_t i = 0; i < not_bridges.size(); i++){
      disjoint.async_union(not_bridges.at(i).first, not_bridges.at(i).second);
    }
    world.barrier();
    
    for(int i = 0; i < maybe_bridges.size(); i++){
      pair<long long, long long> edge = maybe_bridges.at(i);
      disjoint.async_union_and_execute(edge.first, edge.second, [](const long long& vertex_one, const long long& vertex_two, bool merged){
        if(!merged){
          not_bridges.push_back(pair<long long, long long>(vertex_one, vertex_two));
        }
        else{
          new_maybe_bridges.push_back(pair<long long, long long>(vertex_one, vertex_two));
        }
      });
    }
    world.barrier();
    const auto end{std::chrono::steady_clock::now()};
    const std::chrono::duration<double> elapsed_seconds{end - start};
    //world.cout0("Time spent on union find stuff: ", elapsed_seconds, "\n"); 
    size_t total_maybe_bridges_size = ygm::sum(maybe_bridges.size(), world);
    size_t total_new_bridges_size = ygm::sum(new_maybe_bridges.size(), world);
    world.barrier();
    if(total_maybe_bridges_size == total_new_bridges_size){
      break;
    }
    maybe_bridges = new_maybe_bridges;
    new_maybe_bridges.clear();
    disjoint.clear();
    num_iterations++;
    world.barrier();
    const auto new_end{std::chrono::steady_clock::now()};
    const std::chrono::duration<double> new_elapsed_seconds{new_end - end};
    //world.cout0("Time spend on gathering: ", new_elapsed_seconds, "\n"); 
  }
  size_t total_bridges = ygm::sum(maybe_bridges.size(), world);
  world.cout0("total bridges: ", total_bridges);
  world.cout0("total iterations: ", num_iterations);
  world.barrier();
}


struct Edge{
	long long vertex_one;
	long long vertex_two;
	long long degree_one;
	long long degree_two;
  Edge() : vertex_one(0), vertex_two(0), degree_one(0), degree_two(0) {}
	Edge(long long vertex_one, long long vertex_two, long long degree_one, long long degree_two): vertex_one(vertex_one), vertex_two(vertex_two), degree_one(degree_one), degree_two(degree_two){}
  template <class Archive>
  void serialize(Archive& ar) {
      ar(vertex_one, vertex_two, degree_one, degree_two);
  }
};
bool compare_small(const Edge a, const Edge b){
  return ((a.degree_one - 1) * (a.degree_two - 1)) < ((b.degree_one - 1) * (b.degree_two - 1));
}
bool compare_large(const Edge a, const Edge b){
  return ((a.degree_one -1) * (a.degree_two -1)) > ((b.degree_one -1) * (b.degree_two - 1));
}
void find_bridges_parallel_opt(string& csv_file, ygm::comm& world){
  //csv file must be in the format:
  //<vertex one>,<vertex two>
  //<vertex one>,<vertex two>
  //...
  //<vertex one>,<vertex two>
  //and should not contain duplicates
  static ygm::comm* s_world = &world;
  vector<string> filenames(1, csv_file);
  ygm::io::csv_parser parser(world, filenames);
  static vector<Edge> not_bridges;
  static vector<Edge> maybe_bridges;
	static ygm::container::map<long long, long long> vertex_degree_mapping(world);
  static auto map_increment_function = [](const long long& key, const long long& value){
    vertex_degree_mapping.local_insert_or_assign(key, value + 1);
  }; 
  parser.for_all([](ygm::io::detail::csv_line line){
    long long vertex_one = line[0].as_integer();
    long long vertex_two = line[1].as_integer();
    vertex_degree_mapping.async_visit(vertex_one, map_increment_function);
    vertex_degree_mapping.async_visit(vertex_two, map_increment_function);
    maybe_bridges.push_back(Edge(vertex_one, vertex_two, 0, 0)); 
  });

  std::vector<ygm::ygm_ptr<long long>> degree_one_ptrs;
  std::vector<ygm::ygm_ptr<long long>> degree_two_ptrs;

  for (size_t i = 0; i < maybe_bridges.size(); i++) {
      degree_one_ptrs.push_back(ygm::ygm_ptr<long long>(&maybe_bridges.at(i).degree_one));
      degree_two_ptrs.push_back(ygm::ygm_ptr<long long>(&maybe_bridges.at(i).degree_two));
  }

  static auto update_degree = [](const long long& key,const long long& value, const ygm::ygm_ptr<long long>& vertex_degree){
    *vertex_degree = value;
  };
  for(size_t i = 0; i < maybe_bridges.size(); i++){
    degree_one_ptrs.at(i).check(world);
    degree_two_ptrs.at(i).check(world);
    vertex_degree_mapping.async_visit(maybe_bridges.at(i).vertex_one, update_degree, degree_one_ptrs.at(i));
    vertex_degree_mapping.async_visit(maybe_bridges.at(i).vertex_two, update_degree, degree_two_ptrs.at(i));
  }  
  int num_iterations = 0;
  world.barrier();
  world.cout0("got to main loop");
  while(true){
    static ygm::container::disjoint_set<long long> disjoint(world); //use async_union_and_execute
    static vector<Edge> new_maybe_bridges;
    const auto start{std::chrono::steady_clock::now()};
    for(size_t i = 0; i < not_bridges.size(); i++){
      disjoint.async_union(not_bridges.at(i).vertex_one, not_bridges.at(i).vertex_two);
    }
    world.barrier();
    //edges connected to vertices with small degrees are more likely to be
    //bridges
    if(num_iterations % 2 == 0){
      std::sort(maybe_bridges.begin(), maybe_bridges.end(), compare_small);
    }
    else{
      std::sort(maybe_bridges.begin(), maybe_bridges.end(), compare_large);
    }
    world.cout0("first edge degrees: ", maybe_bridges.at(0).degree_one, " ", maybe_bridges.at(0).degree_two);
    world.cout0("last edge degrees: ", maybe_bridges.at(maybe_bridges.size()-1).degree_one, " ", maybe_bridges.at(maybe_bridges.size() -1).degree_two);
    for(int i = 0; i < maybe_bridges.size(); i++){
      Edge edge = maybe_bridges.at(i);
      disjoint.async_union_and_execute(edge.vertex_one, edge.vertex_two, [](const long long& vertex_one, const long long& vertex_two, bool merged, const Edge& my_edge){
        if(!merged){
          not_bridges.push_back(my_edge);
        }
        else{
          new_maybe_bridges.push_back(my_edge);
        }
      }, edge);
    }
    world.barrier();
    const auto end{std::chrono::steady_clock::now()};
    const std::chrono::duration<double> elapsed_seconds{end - start};
    //world.cout0("Time spent on union find stuff: ", elapsed_seconds, "\n"); 
    size_t total_maybe_bridges_size = ygm::sum(maybe_bridges.size(), world);
    size_t total_new_bridges_size = ygm::sum(new_maybe_bridges.size(), world);
    world.barrier();
    if(total_maybe_bridges_size == total_new_bridges_size){
      break;
    }
    maybe_bridges = new_maybe_bridges;
    new_maybe_bridges.clear();
    disjoint.clear();
    num_iterations++;
    world.barrier();
    const auto new_end{std::chrono::steady_clock::now()};
    const std::chrono::duration<double> new_elapsed_seconds{new_end - end};
    //world.cout0("Time spend on gathering: ", new_elapsed_seconds, "\n"); 
  }
  size_t total_bridges = ygm::sum(maybe_bridges.size(), world);
  world.cout0("total bridges: ", total_bridges);
  world.cout0("total iterations: ", num_iterations);
  world.barrier();
}
