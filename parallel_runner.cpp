#include "bridges.h"
using std::string, std::cout;
int main(int argc, char** argv){
  ygm::comm world(&argc, &argv);
  world.welcome();
  if(world.rank0()){
    if(argc < 2){
      cout << "need to provide file name\n";
      return 1;
    }
  }
  string file_name(argv[1]);
  const auto start{std::chrono::steady_clock::now()};
  find_bridges_parallel(file_name, world);
  const auto finish{std::chrono::steady_clock::now()};
  const std::chrono::duration<double> elapsed_seconds{finish - start};
  cout << file_name << " took " << elapsed_seconds << "\n";
}
