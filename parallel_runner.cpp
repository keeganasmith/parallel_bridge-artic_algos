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
  find_bridges_parallel(file_name, world);
}
