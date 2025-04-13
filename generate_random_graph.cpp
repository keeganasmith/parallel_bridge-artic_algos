#include <bits/stdc++.h>
#include <omp.h>
using namespace std;
struct Edge{
    long long vertex_one;
    long long vertex_two;
};

int generate_random_vertex(long long num_vertices){
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<long long> dis(0, num_vertices - 1);
    return dis(gen);
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
    set<pair<long long, long long>> already_added;
    while(num_vertices <= max_num_vertices){
        
        string file_name = "./graphs/" + to_string(num_vertices) + "_" + to_string(average_degree) + ".csv";
        if(filesystem::exists(file_name)){
          num_vertices *= 2;
          continue;
        }
        poisson_distribution<int> degree_dist(average_degree);
        vector<Edge> vertex_edges;
        #pragma omp parallel
        {
            int thread_id = omp_get_thread_num();
            #pragma omp for
            for(long long i = 0; i < num_vertices; i++){
                mt19937 gen(thread_id);
                int num_children = degree_dist(gen);
                for(long long j = 0; j < num_children; j++){
                    Edge my_edge;
                    my_edge.vertex_one = i;
                    my_edge.vertex_two = generate_random_vertex(num_vertices);
                    while(my_edge.vertex_one == my_edge.vertex_two){
                      my_edge.vertex_two = generate_random_vertex(num_vertices);
                    }
                    //the following is to guarantee uniqueness since undirected
                    //graph
                    #pragma omp critical
                    {
                    if(already_added.count(pair<long long, long long>(my_edge.vertex_one, my_edge.vertex_two)) == 0){
                      vertex_edges.push_back(my_edge);
                      already_added.emplace(pair<long long, long long>(my_edge.vertex_one, my_edge.vertex_two));
                      
                    }
                    }
                }
            }
        }
        ofstream out(file_name);
        if (!out.is_open()) {
            cerr << "Failed to open " << file_name;
            return 1;
        }
        for (long long i = 0; i < vertex_edges.size(); ++i) {
          Edge edge = vertex_edges.at(i);
          out << edge.vertex_one << "," << edge.vertex_two << "\n";
        }
        out.close();
        cout << "finished graph with " << num_vertices << " vertices\n";
        num_vertices *= 2;
    }

}   
