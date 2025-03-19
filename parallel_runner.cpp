#include "bridges.h"
using std::string, std::cout;
int main(int argc, char** argv){
  if(argc < 2){
    cout << "need to provide file name\n";
  }
  string file_name(argv[1]);
  cout << "executing...\n";
  find_bridges_parallel(file_name);
}
