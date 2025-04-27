#include <bits/stdc++.h>
#include <omp.h>
using namespace std;
struct Edge{
    long long vertex_one;
    long long vertex_two;
};
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
void generate_graph(long long num_vertices, long long average_degree){
    set<pair<long long, long long>> already_added;
    string file_name = "./graphs/" + to_string(num_vertices) + "_" + to_string(average_degree) + ".csv";
    if(filesystem::exists(file_name)){
      return;
    }
    vector<Edge> vertex_edges;
    int thread_id = omp_get_thread_num();
    for(long long i = 0; i < num_vertices * average_degree; i++){
        mt19937 gen(thread_id);
        Edge my_edge;
        my_edge.vertex_one = generate_random_vertex(num_vertices);
        my_edge.vertex_two = generate_random_vertex(num_vertices);
        while(my_edge.vertex_one == my_edge.vertex_two){
          my_edge.vertex_two = generate_random_vertex(num_vertices);
        }
        //the following is to guarantee uniqueness since undirected
        //graph
        if(already_added.count(pair<long long, long long>(my_edge.vertex_one, my_edge.vertex_two)) == 0){
          vertex_edges.push_back(my_edge);
          already_added.emplace(pair<long long, long long>(my_edge.vertex_one, my_edge.vertex_two));
          
        }
    }
    ofstream out(file_name);
    if (!out.is_open()) {
        cerr << "Failed to open " << file_name;
        return;
    }
    for (long long i = 0; i < vertex_edges.size(); ++i) {
      Edge edge = vertex_edges.at(i);
      out << edge.vertex_one << "," << edge.vertex_two << "\n";
    }
    out.close();
    cout << "finished graph with " << num_vertices << " vertices\n";

}

int main(int argc, char** argv){
    omp_set_num_threads(192);
    if(argc < 3){
        cout << "need to provide max number of vertices and average degree\n";
        return 1;
    }
    long long max_num_vertices = stol(argv[1]);
    long long num_vertices = 131072;
    long long average_degree = stol(argv[2]);
    int max_power = int(floor(log_base(max_num_vertices / num_vertices, 2)));
    #pragma omp parallel for
    for(long long power = 1; power <= max_power; power++) {
      generate_graph(num_vertices * int_pow(2, power), average_degree);  
    }
    
    #pragma omp parallel for
    for(long long vertices = num_vertices * 192; vertices <= 192 * 4 * num_vertices; vertices += 192 * num_vertices){
      generate_graph(vertices, average_degree);  
    }


}   
