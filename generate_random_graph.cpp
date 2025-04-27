#include <bits/stdc++.h>
#include <ygm/comm.hpp>
#include <ygm/container/map.hpp>
#include <ygm/container/set.hpp>
#include <ygm/container/bag.hpp>
using namespace std;
namespace std {
template <>
struct hash<std::pair<long long, long long>> {
    size_t operator()(const std::pair<long long, long long>& p) const {
        return hash<long long>()(p.first) ^ (hash<long long>()(p.second) << 1);
    }
};
}
long long int_pow(long long base, int exp) {
    long long result = 1;
    for (int i = 0; i < exp; ++i) {
        result *= base;
    }
    return result;
}
double log_base(double x, double base) {
    return log(x) / log(base);
}
int generate_random_vertex(long long num_vertices){
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<long long> dis(0, num_vertices - 1);
    return dis(gen);
}
void generate_graph(long long num_vertices, long long average_degree, ygm::comm& world){
    static ygm::container::set<pair<long long, long long>> already_added(world);
    size_t world_size = world.size();
    world.cout0("world size is: ", world_size);
    int rank = world.rank();
    world.cout0("got to generate graphs: ", num_vertices);
    string file_name = "./graphs/" + to_string(num_vertices) + "_" + to_string(average_degree) + ".csv";
    if(filesystem::exists(file_name)){
      return;
    }
    static vector<pair<long long, long long>> vertex_edges;
    long long total_edges = num_vertices * average_degree / 2; // divide by two, undirected so each edge increases degree of both vertices
    long long my_num_edges = total_edges / world_size; // in the worst case we trim world_size edges, not a big deal
    world.cout0("my edges: ", my_num_edges);
    for(long long i = 0; i < my_num_edges; i++){ 
        mt19937 gen(rank);
        long long vertex_a = generate_random_vertex(num_vertices);
        long long vertex_b = generate_random_vertex(num_vertices);
        while(vertex_a == vertex_b){
          vertex_b = generate_random_vertex(num_vertices);
        }
        //the following is to guarantee uniqueness since undirected
        //graph
        long long vertex_one = min(vertex_a, vertex_b);
        long long vertex_two = max(vertex_a, vertex_b);
        pair<long long, long> edge(vertex_one, vertex_two);
        already_added.async_contains(edge, [](bool found, const pair<long long, long long>& edge){
              if(!found){
                vertex_edges.push_back(edge);
                already_added.async_insert(edge);
              }

            });
        if(i % (my_num_edges / 10) == 0){
          world.cout0("finished ", i," edges");
        }
    }
    world.barrier();
    world.cout0("generating file");
    vector<char> buffer;
    for(pair<long long, long long> edge : vertex_edges){
      string line = to_string(edge.first) + "," + to_string(edge.second) + "\n";
      buffer.insert(buffer.end(), line.begin(), line.end());
    }
    MPI_File fh;
    MPI_File_open(MPI_COMM_WORLD, file_name.c_str(), MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fh);

    MPI_Offset local_size = buffer.size();
    MPI_Offset local_offset = 0;
    MPI_Exscan(&local_size, &local_offset, 1, MPI_OFFSET, MPI_SUM, MPI_COMM_WORLD);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        local_offset = 0;
    }

    MPI_File_write_at_all(fh, local_offset, buffer.data(), local_size, MPI_CHAR, MPI_STATUS_IGNORE);

    MPI_File_close(&fh);

    world.barrier();
    world.cout0("finished graph with ", num_vertices, " vertices");

}

int main(int argc, char** argv){
    ygm::comm world(&argc, &argv);
    world.welcome();
    if(argc < 3){
        world.cout0("need to provide max number of vertices and average degree\n");
        return 1;
    }
    long long max_num_vertices = stol(argv[1]);
    long long num_vertices = 131072;
    long long average_degree = stol(argv[2]);
    int max_power = int(floor(log_base(max_num_vertices / num_vertices, 2)));
    for(long long power = 0; power <= max_power; power++) {
      world.cout0("generating graph");
      world.cout0(num_vertices);
      world.cout0(int_pow(2, power));
      world.cout0(num_vertices * int_pow(2, power));
      generate_graph(num_vertices * int_pow(2, power), average_degree, world);  
      world.barrier();
    }
    num_vertices = 524288; 
    for(long long vertices = num_vertices * 192; vertices <= 192 * 4 * num_vertices; vertices += 192 * num_vertices){
      generate_graph(vertices, average_degree, world);  
      world.barrier();
    }


}   
