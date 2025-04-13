#include "bridges.h"
using namespace std;
vector<pair<long long, long long>> parse_graph_csv(string file_name){
  vector<pair<long long, long long>> result;
  ifstream csv_file(file_name);
  string line;
  while(getline(csv_file, line)){
    stringstream ss(line);
    string vertex_one_str;
    getline(ss, vertex_one_str, ',');
    string vertex_two_str;
    ss >> vertex_two_str;
    long long vertex_one = stol(vertex_one_str);
    long long vertex_two = stol(vertex_two_str);
    result.push_back(pair<long long, long long>(vertex_one, vertex_two));
  }
  return result;
}
int main(int argc, char** argv){
  long long size = stol(argv[1]);
  long long degree = stol(argv[2]);
  string file_name = "./graphs/" + to_string(size) + "_" + to_string(degree) + ".csv";
  vector<pair<long long, long long>> edge_list = parse_graph_csv(file_name);
  vector<pair<long long, long long>> result = find_bridges(edge_list, size);
  cout << "num bridges: " << result.size();
}
