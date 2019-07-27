//
// Created by vicky on 25/07/19.
//

#include <cassert>
#include <iostream>
#include <string>

#include "assignments/dg/graph.h"

int main() {
  gdwg::Graph<std::string, double> g;
  g.InsertNode("a");
  g.InsertNode("b");
  g.InsertNode("c");

  assert(g.InsertEdge("b", "c", 1));
  assert(g.InsertEdge("b", "c", 2));

  assert(g.InsertEdge("b", "a", 2.8));
  assert(g.InsertEdge("b", "a", 1.5));

  assert(g.InsertEdge("a", "b", 1));
  assert(g.InsertEdge("a", "b", 2));
  assert(g.InsertEdge("a", "c", 1));
  assert(g.InsertEdge("a", "c", 2));

  for (auto it = g.cbegin(); it != g.cend(); ++it) {
    std::cout << std::get<0>(*it) << " " << std::get<1>(*it) << " " << std::get<2>(*it)
              << std::endl;
  }
  return 0;
}
