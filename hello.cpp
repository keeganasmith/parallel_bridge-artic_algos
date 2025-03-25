#include <bits/stdc++.h>
#include <ygm/comm.hpp>
#include <ygm/comm.hpp>
#include <ygm/container/map.hpp>
#include <ygm/container/set.hpp>
#include <ygm/container/bag.hpp>
#include <ygm/container/disjoint_set.hpp>
#include <ygm/io/csv_parser.hpp>

int main(int argc, char** argv){
  ygm::comm world(&argc, &argv);
  world.welcome();
  ygm::container::bag<int> not_bridges(world);
  ygm::container::bag<int> maybe_bridges(world);
  
  maybe_bridges = not_bridges;
}
