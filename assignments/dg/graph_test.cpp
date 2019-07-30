/*

  == Explanation and rational of testing ==

  Explain and justify how you approached testing, the degree
   to which you're certain you have covered all possibilities,
   and why you think your tests are that thorough.

*/

#include <algorithm>
#include <iostream>
#include <random>
#include <string>

#include "assignments/dg/graph.h"
#include "catch.h"

std::random_device rd;
std::mt19937 random_engine(rd());

SCENARIO("Iterator test") {
  GIVEN("Graph<int, double> with nodes 0, 1, 2, 3 and edges 1 -> 1: 0.5; 1 -> 1: 1.5; 1 -> 2: 0; "
        "2 -> 2: 0.1") {
    std::vector<std::tuple<int, int, double>> edges{
        {1, 1, 0.5}, {1, 1, 1.5}, {1, 2, 0}, {2, 2, 0.1}};
    /* shuffle order to test iterator order */
    std::shuffle(edges.begin(), edges.end(), random_engine);
    gdwg::Graph<int, double> g{edges.begin(), edges.end()};
    g.InsertNode(0); /* leading node without connection */
    g.InsertNode(3); /* trailing node without connection */
    WHEN("Getting iterators using begin") {
      auto it = g.begin();
      THEN("Should return 4 edges") { REQUIRE(std::distance(g.begin(), g.end()) == 4); }
      THEN("They should be in the order of src, dst, weight") {
        REQUIRE(*it == std::make_tuple(1, 1, 0.5));
        REQUIRE(*++it == std::make_tuple(1, 1, 1.5));
        REQUIRE(*++it == std::make_tuple(1, 2, 0));
        REQUIRE(*++it == std::make_tuple(2, 2, 0.1));
      }
    }
    WHEN("Getting iterators using rbegin") {
      auto it = g.rbegin();
      THEN("Should return 4 edges") { REQUIRE(std::distance(g.rbegin(), g.rend()) == 4); }
      THEN("They should be in the reverse order of src, dst, weight") {
        REQUIRE(*it == std::make_tuple(2, 2, 0.1));
        REQUIRE(*++it == std::make_tuple(1, 2, 0));
        REQUIRE(*++it == std::make_tuple(1, 1, 1.5));
        REQUIRE(*++it == std::make_tuple(1, 1, 0.5));
      }
    }
    WHEN("using find to find edge {1 -> 1: 0.5}") {
      auto it = g.find(1, 1, 0.5);
      THEN("Should give iterator to that edge") {
        REQUIRE(it != g.cend());
        REQUIRE(*it == std::make_tuple(1, 1, 0.5));
      }
    }
    WHEN("using find to find non-existent edge {1 -> 1: 0.6}") {
      auto it = g.find(1, 1, 0.6);
      THEN("Should give cend()") { REQUIRE(it == g.cend()); }
    }
    WHEN("using find to find non-existent edge {0 -> 0: 0.5}") {
      auto it = g.find(0, 0, 0.5);
      THEN("Should give cend()") { REQUIRE(it == g.cend()); }
    }
    WHEN("erase edge 1 -> 2: 0") {
      THEN("should return true") { REQUIRE(g.erase(1, 2, 0)); }
    }
    WHEN("Using iterator to erase 1 -> 1: 0.5") {
      THEN("should return iterator to 1 -> 1: 1.5") {
        auto it = g.erase(g.cbegin());
        REQUIRE(*it == std::make_tuple(1, 1, 1.5));
        REQUIRE(std::distance(g.begin(), g.end()) == 3);
      }
    }
    WHEN("Using iterator to erase 1 -> 2: 0") {
      THEN("should return iterator to 2 -> 2: 0.1") {
        auto it = g.cend();
        std::advance(it, -2);
        it = g.erase(it);
        REQUIRE(*it == std::make_tuple(2, 2, 0.1));
        REQUIRE(std::distance(g.begin(), g.end()) == 3);
      }
    }
    WHEN("Using iterator to erase last edge 2 -> 2: 0.1") {
      THEN("should return .cend()") {
        auto it = g.erase(--g.cend());
        REQUIRE(it == g.cend());
        REQUIRE(std::distance(g.begin(), g.end()) == 3);
      }
    }
  }
  GIVEN("Empty const graph g") {
    const gdwg::Graph<int, int> g;
    WHEN("Getting cbegin()") {
      THEN("Should be the same as cend()") {
        REQUIRE(g.cbegin() == g.cend());
      }
    }
    WHEN("finding edge 1 1 1") {
      THEN("Should return cend()") {
        REQUIRE(g.find(1, 1, 1) == g.cend());
      }
    }
  }
}

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
  GIVEN("a list of nodes with weight") {
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
  GIVEN("simple graaph") {
    std::string s1{"hello"};
    std::string s2{"how"};
    std::string s3{"are"};
    auto e1 = std::make_tuple(s1, s2, 5.4);
    auto e2 = std::make_tuple(s2, s3, 7.6);
    auto e = std::vector<std::tuple<std::string, std::string, double>>{e1, e2};
    gdwg::Graph<std::string, double> b{e.begin(), e.end()};
    WHEN("copy this graph") {
      gdwg::Graph<std::string, double> copyG{b};
      std::vector<std::string> s{"hello", "how", "are"};
      sort(s.begin(), s.end());
      REQUIRE(copyG.GetNodes() == s);
    }
  }
}
