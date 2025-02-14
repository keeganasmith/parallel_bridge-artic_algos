#pragma once
#include <bits/stdc++.h>
using std::vector;
struct Node{
  int parent;
  int rank;
};
class Union_Find {
  private:
  vector<Node> nodes;
  public:
  Union_Find();
  Union_Find(size_t size);
  bool merge(size_t index_one, size_t index_two);
  size_t find(size_t index);
};
