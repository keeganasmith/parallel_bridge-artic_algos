#include "bridges.h"
using std::cout, std::vector, std::pair, std::endl, std::max, std::min;
namespace std {
  template<>
  struct hash<std::pair<long long,long long>> {
    size_t operator()(std::pair<long long,long long> const& p) const noexcept {
      // e.g. boost’s combine, or a simple mix:
      return (uint64_t(p.first) * 11400714819323198485ULL) ^ (uint64_t(p.second) * 14029467366897019727ULL);
    }
  };
}
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
long get_outlier(ygm::container::bag<pair<long long, long long>>& my_bag, ygm::comm& world){
  long total_size = my_bag.size();
  long local_size = my_bag.local_size();
  double average_size = (double)total_size / (double)world.size();
  long difference = abs((long)average_size - local_size);
  long max_diff = ygm::max(difference, world);
  return max_diff; 
}
void find_bridges_parallel(string& csv_file, ygm::comm& world){
  //csv file must be in the format:
  //<vertex one>,<vertex two>
  //<vertex one>,<vertex two>
  //...
  //<vertex one>,<vertex two>
  //and should not contain duplicates
  static ygm::comm* s_world = &world;
  size_t world_size = world.size();
  vector<string> filenames(1, csv_file);
  ygm::io::csv_parser parser(world, filenames);
  static ygm::container::bag<pair<long long, long long>> not_bridges(world);
  static ygm::container::bag<pair<long long, long long>> maybe_bridges(world);
  parser.for_all([](ygm::io::detail::csv_line line){
    long long vertex_one = line[0].as_integer();
    long long vertex_two = line[1].as_integer();
    maybe_bridges.async_insert(pair<long long, long long>(vertex_one, vertex_two)); 
  });
  int num_iterations = 0;
  world.barrier();
  long maybe_max_diff = get_outlier(maybe_bridges, world);
  world.cout0("maybe max diff: ", maybe_max_diff);
  while(true){
    static ygm::container::disjoint_set<long long> disjoint(world); //use async_union_and_execute
    static ygm::container::bag<pair<long long, long long>> new_maybe_bridges(world);
    const auto start{std::chrono::steady_clock::now()};
    const auto not_bridges_loop = [](const pair<long long, long long>& edge){
      disjoint.async_union(edge.first, edge.second);
    };
    //world.cout0("executing not bridges loop");
    not_bridges.for_all(not_bridges_loop);
    world.barrier();
    const auto maybe_bridges_loop = [](const pair<long long, long long>& edge){
      disjoint.async_union_and_execute(edge.first, edge.second, [](const long long& vertex_one, const long long& vertex_two, bool merged){
        if(!merged){
          not_bridges.async_insert(pair<long long, long long>(vertex_one, vertex_two));
        }
        else{
          new_maybe_bridges.async_insert(pair<long long, long long>(vertex_one, vertex_two));
        }
      });
    }; 
    //world.cout0("executing maybe bridges loop");
    maybe_bridges.for_all(maybe_bridges_loop);
    world.barrier();
    const auto end{std::chrono::steady_clock::now()};
    const std::chrono::duration<double> elapsed_seconds{end - start};
    //world.cout0("Time spent on union find stuff: ", elapsed_seconds, "\n"); 
    size_t total_maybe_bridges_size = maybe_bridges.size();
    size_t total_new_bridges_size = new_maybe_bridges.size();
    world.barrier();
    if(total_maybe_bridges_size == total_new_bridges_size){
      break;
    }
    world.barrier();
    maybe_bridges.swap(new_maybe_bridges);
    maybe_max_diff = get_outlier(maybe_bridges, world);
    world.cout0("maybe max diff: ", maybe_max_diff);
    long not_max_diff = get_outlier(not_bridges, world);
    world.cout0("not max diff: ", not_max_diff);
    //world.cout0("new maybe bridges size: ", total_new_bridges_size);
    //world.cout0("not bridges size: ", not_bridges.size());
    new_maybe_bridges.clear();
    disjoint.clear();
    num_iterations++;
    world.barrier();
    const auto new_end{std::chrono::steady_clock::now()};
    const std::chrono::duration<double> new_elapsed_seconds{new_end - end};
    //world.cout0("Time spend on gathering: ", new_elapsed_seconds, "\n"); 
  }
  size_t total_bridges = maybe_bridges.size();
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
void initialize_ccids_parents(ygm::container::bag<pair<long long, long long>>& edges, ygm::container::map<long long, long long>& ccids, ygm::container::map<long long, long long>& parents, ygm::comm& world){
  static ygm::container::map<long long, long long>* s_ccids;
  static ygm::container::map<long long, long long>* s_parents;
  s_ccids = &ccids;
  s_parents = &parents;
  auto edges_loop = [](const pair<long long, long long>& edge){
    s_ccids->async_insert_or_assign(edge.first, edge.first);
    s_ccids->async_insert_or_assign(edge.second, edge.second);
    s_parents->async_insert_or_assign(edge.first, edge.first);
    s_parents->async_insert_or_assign(edge.second, edge.second);
  };
  edges.for_all(edges_loop);
  world.barrier();
}

void label_propagation(ygm::container::set<pair<long long, long long>>& edges, ygm::container::map<long long, long long>& ccids, ygm::container::map<long long, long long>& parents, ygm::container::bag<pair<long long, long long>>& spanning_tree, ygm::comm& world){
  /*
  updated = true;
  while(updated)
    updated = false;
    for [u, v] in edges:
      if(ccids[u] < ccids[v]):
        ccids[v] = ccids[u]
        parents[v] = u
        updated = true;
      if(ccids[v] < ccids[u]):
        ccids[u] = ccids[v]
        parents[u] = v
        updated = true;
  
  spanning tree edges = []
  for(int i = 0; i < parents.size(); i++):
    edge = [i, parents[i]]
    spanning tree edges += edge
  */
  static ygm::container::set<pair<long long, long long>>* s_edges;;
  static ygm::container::map<long long, long long>* s_ccids;
  static ygm::container::map<long long, long long>* s_parents;
  static ygm::container::bag<pair<long long, long long>>* s_spanning_tree;
  static ygm::comm* s_world = &world;
  s_edges = &edges;
  s_ccids = &ccids;
  s_parents = &parents;
  s_spanning_tree = &spanning_tree;
  static bool local_updated;
  local_updated = true;
  while(local_updated){
    local_updated = false;
    auto edge_loop_function = [](const pair<long long, long long>& edge){
      s_ccids->async_visit(edge.first, [](const long long& key, const long long& value, const pair<long long, long long>& edge){
        long long u_ccid = value;
        s_ccids->async_visit(edge.second, [](const long long& key, const long long& value, const long long& u_ccid, const pair<long long, long long>& edge){
          long long v_ccid = value;
          //edge is u, v
          if(u_ccid < v_ccid){
            s_ccids->async_insert_or_assign(edge.second, u_ccid);
            s_parents->async_insert_or_assign(edge.second, edge.first);
            local_updated = true;
          } 
          else if(v_ccid < u_ccid){
            s_ccids->async_insert_or_assign(edge.first, v_ccid);
            s_parents->async_insert_or_assign(edge.first, edge.second);
            local_updated = true;
          }
        }, u_ccid, edge);
      }, edge);
    };
    edges.for_all(edge_loop_function);
    world.barrier();
    //if any proc has local updated, need to continue, reduce all or
    bool local_updated = world.all_reduce(local_updated, [](const bool& one, const bool& two){
      return one || two;
    });
    world.barrier();
  }
  spanning_tree.clear();
  auto parents_loop = [](const long long& child_vertex, const long long& parent_vertex){
    if(child_vertex != parent_vertex){
      pair<long long, long long> edge(min(child_vertex, parent_vertex), max(child_vertex, parent_vertex));
      s_spanning_tree->async_insert(edge);
    }
  };
  s_parents->for_all(parents_loop);
  world.barrier();

}


void find_bridges_parallel_opt(string& csv_file, ygm::comm& world){
  static ygm::comm* s_world = &world;
  size_t world_size = world.size();
  vector<string> filenames(1, csv_file);
  ygm::io::csv_parser parser(world, filenames);
  static ygm::container::set<pair<long long, long long>> not_bridges(world);
  static ygm::container::set<pair<long long, long long>> maybe_bridges(world);
  static ygm::container::bag<pair<long long, long long>> edges(world);
  parser.for_all([](ygm::io::detail::csv_line line){
    long long vertex_a = line[0].as_integer();
    long long vertex_b = line[1].as_integer();
    long long vertex_one = min(vertex_a, vertex_b);
    long long vertex_two = max(vertex_a, vertex_b);
    edges.async_insert(pair<long long, long long>(vertex_one, vertex_two));
    maybe_bridges.async_insert(pair<long long, long long>(vertex_one, vertex_two)); 
  });
  while(true){
    //label propogation
    /*
    ccids = []
    parents = []
    for [u, v] in edges:
      ccid[u] = u
      ccid[v] = v
      parents[u] = u
      parents[v] = v
    
    updated = true;
    while(updated)
      updated = false;
      for [u, v] in edges:
        if(ccids[u] < ccids[v]):
          ccids[v] = ccids[u]
          parents[v] = u
          updated = true;
        if(ccids[v] < ccids[u]):
          ccids[u] = ccids[v]
          parents[u] = v
          updated = true;
    
    spanning tree edges = []
    for(int i = 0; i < parents.size(); i++):
      edge = [i, parents[i]]
      spanning tree edges += edge
    return spanning tree_edges


    label_propagation(not_bridge_edges, ccids, parents, dummy_spanning_tree_edges)
    # do nothing with spanning_tree_edges
    world.barrier()
    
    label_propagation(maybe_bridge_edges, ccids, parents, spanning_tree_edges)
    maybe_bridges = spanning_tree_edges not in not bridges, i.e:
    for edge in spanning_tree_edges:
      if(edge not in not bridges):
        maybe_bridges += edge
    
    not_bridges = edges - maybe_bridges, i.e: 
    for edge in edges: 
      if edge not in maybe_bridges:
        not_bridges += edge
    
    if not bridges is the same size, break, else repeat

    to make this efficient, we need to make not_bridges and maybe_bridges sets
    */
    ygm::container::map<long long, long long> ccids(world);
    ygm::container::map<long long, long long> parents(world);
    ygm::container::bag<pair<long long, long long>> spanning_tree(world);
    initialize_ccids_parents(edges, ccids, parents, world);
    world.cout0("doing label propagation with not_bridges size: ", not_bridges.size());
    label_propagation(not_bridges, ccids, parents, spanning_tree, world);
    world.barrier();
    world.cout0("spanning tree size after not bridges: ", spanning_tree.size());
    world.cout0("maybe bridges size for label propagation is: ", maybe_bridges.size());
    label_propagation(maybe_bridges, ccids, parents, spanning_tree, world);
    world.barrier();
    maybe_bridges.clear();
    world.cout0("spanning tree size: ", spanning_tree.size());
    auto spanning_tree_loop = [](const pair<long long, long long>& edge){
      not_bridges.async_contains(edge, [](bool present, const pair<long long, long long>& edge){
        if(!present){
          maybe_bridges.async_insert(edge);
        }
        else{
          s_world->cout0("edge was found in not edges, so ignoring");
        }
      });
    };
    spanning_tree.for_all(spanning_tree_loop);
    world.barrier();
    world.cout0("maybe bridges size: ", maybe_bridges.size());
    size_t old_not_bridge_size = not_bridges.size();
    not_bridges.clear();
    auto edges_loop = [](const pair<long long, long long>& edge){
      size_t edge_count_in_maybe_bridges = maybe_bridges.count(edge);
      maybe_bridges.async_contains(edge, [](bool present, const pair<long long, long long>& edge){
        if(!present){
          not_bridges.async_insert(edge);
        }
      });
    };
    edges.for_all(edges_loop);
    world.barrier();
    size_t new_not_bridge_size = not_bridges.size();
    if(new_not_bridge_size == old_not_bridge_size){
      break;
    }
  }
  world.barrier();
  world.cout0("Num bridges: ", maybe_bridges.size());
}

void test_disjoint_set(string& csv_file, ygm::comm& world){
  //csv file must be in the format:
  //<vertex one>,<vertex two>
  //<vertex one>,<vertex two>
  //...
  //<vertex one>,<vertex two>
  //and should not contain duplicates
  static ygm::comm* s_world = &world;
  size_t world_size = world.size();
  vector<string> filenames(1, csv_file);
  ygm::io::csv_parser parser(world, filenames);
  static ygm::container::bag<pair<long long, long long>> maybe_bridges(world);
  parser.for_all([](ygm::io::detail::csv_line line){
    long long vertex_one = line[0].as_integer();
    long long vertex_two = line[1].as_integer();
    maybe_bridges.async_insert(pair<long long, long long>(vertex_one, vertex_two)); 
  });
  world.barrier();
  /*
  long maybe_max_diff = get_outlier(maybe_bridges, world);
  world.cout0("maybe max diff: ", maybe_max_diff);
  */
	static ygm::container::disjoint_set<long long> disjoint(world); //use async_union_and_execute
	static ygm::container::bag<pair<long long, long long>> new_maybe_bridges(world);
	const auto start{std::chrono::steady_clock::now()};
	//world.cout0("executing not bridges loop");
	world.barrier();
	const auto maybe_bridges_loop = [](const pair<long long, long long>& edge){
		disjoint.async_union_and_execute(edge.first, edge.second, [](const long long& vertex_one, const long long& vertex_two, bool merged){});
	}; 
  maybe_bridges.for_all(maybe_bridges_loop);
  world.barrier();
  const auto end{std::chrono::steady_clock::now()};
  const std::chrono::duration<double> new_elapsed_seconds{end - start};
  world.cout0("union find took: ", new_elapsed_seconds, " seconds");
}

