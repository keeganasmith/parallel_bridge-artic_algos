#include "bridges.h"
using std::string, std::cout;
int main(int argc, char** argv){
  ygm::comm world(&argc, &argv);
  if(world.rank0()){
    if(argc < 2){
      cout << "usage: " << "./" << argv[0] << " <file name> <version number [1 | 2 | 3(both)]>";
      return 1;
    }
  }
  string file_name(argv[1]);
  string version(argv[2]);
  const auto start{std::chrono::steady_clock::now()};
  string version_name;
  if(version == "1"){
    version_name = "regular";
    double time_taken = find_bridges_parallel(file_name, world);
    world.cout0(file_name," took ",time_taken," with ", world.size(), " ", version_name);
    world.barrier();
  }
  else if(version == "2"){
    version_name = "optimized";
    find_bridges_parallel_opt(file_name, world);
  }
  else if(version == "3"){
    find_bridges_parallel(file_name, world);
    auto finish{std::chrono::steady_clock::now()};
    std::chrono::duration<double> elapsed_seconds{finish - start};
    world.cout0(file_name, " took ", elapsed_seconds, " with ", world.size(), " regular");
    find_bridges_parallel_opt(file_name, world);
    finish = std::chrono::steady_clock::now();
    elapsed_seconds = finish - start;
    world.cout0(file_name, " took ", elapsed_seconds, " with ", world.size(), " optimized");
    world.barrier();
    return 0;
  }
  else if(version == "4"){
    world.cout0(file_name, " with ", world.size(), " processors: ");
    test_disjoint_set(file_name, world);
    world.barrier();
    return 0;
  }
  else if(version == "5"){
    version_name = "dynamic_programming";
    double time_taken = find_bridges_parallel_dp(file_name, world);
    world.cout0(file_name," took ",time_taken," with ", world.size(), " ", version_name);
    world.barrier();
  } 
  const auto finish{std::chrono::steady_clock::now()};
  const std::chrono::duration<double> elapsed_seconds{finish - start};
  //world.cout0(file_name," took ",elapsed_seconds," with ", world.size(), " ", version_name);
  world.barrier();
}
