/*

  == Explanation and rational of testing ==

  Explain and justify how you approached testing, the degree
   to which you're certain you have covered all possibilities,
   and why you think your tests are that thorough.

*/

#include <iostream>

#include "assignments/dg/graph.h"
#include "catch.h"

SCENARIO("Construct simple graphs") {
  GIVEN("Empty graph g with int nodes/edges") {
    gdwg::Graph<int, int> g;
    WHEN("Insert node 1") {
      REQUIRE(g.InsertNode(1));
      THEN("g.IsNode(1) should return true") { REQUIRE(g.IsNode(1)); }
      THEN("g.IsNode(2) should return false") { REQUIRE(!g.IsNode(2)); }
    }
    WHEN("Insert node 1 twice") {
      THEN("second time should return false") {
        REQUIRE(g.InsertNode(1));
        REQUIRE(g.IsNode(1));
        REQUIRE(!g.InsertNode(1));
      }
    }
  }

  GIVEN("a list of char nodes") {
    gdwg::Graph<char, std::string> g{'a', 'b', 'x', 'y'};
    THEN("node 'a' should be a node") { REQUIRE(g.IsNode('a')); }
  }
  GIVEN("a list of string nodes") {
    std::vector<std::string> v{"hello", "haha"};
    gdwg::Graph<std::string, int> g{v.cbegin(), v.cend()};
    THEN("node 'hello' should be a node") { REQUIRE(g.IsNode("hello")); }
  }
  GIVEN("auto list of nodes with weight") {
    std::string s1{"Hello"};
    std::string s2{"how"};
    std::string s3{"are"};
    auto e1 = std::make_tuple(s1, s2, 5.4);
    auto e2 = std::make_tuple(s2, s3, 7.6);
    auto e = std::vector<std::tuple<std::string, std::string, double>>{e1, e2};
    gdwg::Graph<std::string, double> b{e.begin(), e.end()};
    THEN("should have connection between s1 and s2, s2 and s3") {
      REQUIRE(b.IsNode("Hello"));
      REQUIRE(b.IsNode("how"));
      REQUIRE(!b.IsNode("hehe"));
      REQUIRE(b.IsConnected("Hello", "how"));
      REQUIRE(b.IsConnected("how", "are"));
    }
    WHEN("insert edge 3 between hello and how") {
      THEN("connection exists") { REQUIRE(b.InsertEdge("Hello", "how", 3)); }
    }
    WHEN("insert an exists connection") {
      b.InsertEdge("Hello", "how", 3);
      THEN("fail") { REQUIRE(!b.InsertEdge("Hello", "how", 3)); }
    }
    WHEN("delete a node") {
      b.DeleteNode("Hello");
      THEN("node Hello should not exist") { REQUIRE(!b.IsNode("Hello")); }
    }
  }
}