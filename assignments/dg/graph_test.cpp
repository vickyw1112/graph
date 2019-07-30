/*

  == Explanation and rational of testing ==

  Explain and justify how you approached testing, the degree
   to which you're certain you have covered all possibilities,
   and why you think your tests are that thorough.

*/

#include <iostream>
#include <algorithm>
#include <string>

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
  GIVEN("a list of nodes with weight"){
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
  GIVEN("simple graaph"){
    std::string s1{"hello"};
    std::string s2{"how"};
    std::string s3{"are"};
    auto e1 = std::make_tuple(s1, s2, 5.4);
    auto e2 = std::make_tuple(s2, s3, 7.6);
    auto e = std::vector<std::tuple<std::string, std::string, double>>{e1, e2};
    gdwg::Graph<std::string, double> b{e.begin(), e.end()};

    gdwg::Graph<std::string, double> copyG{b};
    std::vector<std::string> s{"hello", "how", "are"};
    sort(s.begin(), s.end());
    WHEN("copy this graph"){
      THEN("has all nodes"){
        REQUIRE(copyG.GetNodes() == s);
        const std::vector<double> w1{5.4};
        REQUIRE(copyG.GetWeights("hello", "how") == w1);
        const std::vector<double> w2{7.6};
        REQUIRE(copyG.GetWeights("how", "are") == w2);
      }

      WHEN("deletea node in copyG"){
        copyG.DeleteNode("hello");
        std::vector<std::string> nodes{"are", "how"};
        THEN("original graph should have no changes"){
          REQUIRE(copyG.GetNodes() == nodes);
          REQUIRE(b.GetNodes() == s);

          std::vector<double> w1{5.4};
          REQUIRE_THROWS_WITH(copyG.GetWeights("hello", "how"), "Cannot call Graph::GetWeights if src or dst node don't exist in the graph");
          const std::vector<double> w2{7.6};
          REQUIRE(copyG.GetWeights("how", "are") == w2);

          REQUIRE(b.GetWeights("hello", "how") == w1);
        }
      }
    }
  }
}

SCENARIO("test insert edge method"){
  GIVEN("a simple graph"){
    int s1 = 1;
    int s2  = 2;
    int s3 = 3;
    auto e1 = std::make_tuple(s1, s2, "A");
    auto e2 = std::make_tuple(s2, s3, "B");
    auto e = std::vector<std::tuple<int, int, std::string>>{e1, e2};
    gdwg::Graph<int, std::string> b{e.begin(), e.end()};
    WHEN("insert nodes"){
      b.InsertNode(4);
      b.InsertNode(5);
      THEN("4 , 5 are nodes"){
        REQUIRE(b.IsNode(4));
        REQUIRE(b.IsNode(5));
      }
    }
    WHEN("insert edge between an unexist src and dst node"){
      REQUIRE_THROWS_WITH(b.InsertEdge(4, 5, "A"), "Cannot call Graph::InsertEdge when either src or dst node does not exist");
    }
    WHEN("insert edge A between 1 and 2"){
      THEN("fail - dup edge"){
        REQUIRE(!b.InsertEdge(1, 2, "A"));
      }
    }
    WHEN("insert edge B between 1, 2"){
      THEN("success"){
        REQUIRE(b.InsertEdge(1, 2, "B"));
      }
    }
  }
}

SCENARIO("test delete node method"){
  GIVEN("a simple graph"){
    int s1 = 1;
    int s2 = 2;
    int s3 = 3;
    int s4 = 4;
    auto e1 = std::make_tuple(s1, s2, "A");
    auto e2 = std::make_tuple(s2, s3, "B");
    auto e3 = std::make_tuple(s3, s4, "B");
    auto e4 = std::make_tuple(s2, s4, "C");
    auto e = std::vector<std::tuple<int, int, std::string>>{e1, e2, e3, e4};
    gdwg::Graph<int, std::string> b{e.begin(), e.end()};
    WHEN("delete non-exist node"){
      THEN("false"){
        REQUIRE(!b.DeleteNode(5));
      }
    }
    WHEN("delete node 2"){
      REQUIRE(b.DeleteNode(2));
      THEN("connections about node 2 should be removed"){
        REQUIRE(!b.IsNode(2));
        REQUIRE(b.GetConnected(1).empty());
        REQUIRE_THROWS_WITH(b.GetWeights(2, 3), "Cannot call Graph::GetWeights if src or dst node don't exist in the graph");
        REQUIRE_THROWS_WITH(b.GetConnected(2), "Cannot call Graph::GetConnected if src doesn't exist in the graph");
      }
    }
  }
}

SCENARIO("test replace method"){
  int s1 = 1;
  int s2 = 2;
  int s3 = 3;
  int s4 = 4;
  auto e1 = std::make_tuple(s1, s2, "A");
  auto e2 = std::make_tuple(s2, s3, "B");
  auto e3 = std::make_tuple(s3, s4, "B");
  auto e4 = std::make_tuple(s2, s4, "C");
  auto e = std::vector<std::tuple<int, int, std::string>>{e1, e2, e3, e4};
  gdwg::Graph<int, std::string> b{e.begin(), e.end()};
  WHEN("Replace a non-exist node"){
    THEN("throw exception"){
      REQUIRE_THROWS_WITH(b.Replace(5, 4), "Cannot call Graph::Replace on a node that doesn't exist");
    }
  }
  WHEN("replace node 2 with 4"){
    REQUIRE(!b.Replace(2, 4));
  }

  WHEN("replace node 2 with 5"){
    REQUIRE(b.Replace(2, 5));
    THEN("node 2 becomes 5"){
      REQUIRE(!b.IsNode(2));
      REQUIRE(b.IsNode(5));
      std::vector<int> n{1, 3, 4, 5};
      REQUIRE(b.GetNodes() == n);
      std::vector<std::string> w1{"B"};
      REQUIRE(b.GetWeights(5, 3) == w1);
      std::vector<std::string> w2{"C"};
      REQUIRE(b.GetWeights(5, 4) == w2);
      std::vector<std::string> w3{"A"};
      REQUIRE(b.GetWeights(1, 5) == w3);
    }
  }
}

SCENARIO("test mergereplace method"){
  GIVEN("a simple graph"){
    int s1 = 1;
    int s2 = 2;
    int s3 = 3;
    int s4 = 4;
    auto e1 = std::make_tuple(s1, s2, "A");
    auto e2 = std::make_tuple(s2, s3, "B");
    auto e3 = std::make_tuple(s3, s4, "B");
    auto e4 = std::make_tuple(s2, s4, "C");
    auto e = std::vector<std::tuple<int, int, std::string>>{e1, e2, e3, e4};
    gdwg::Graph<int, std::string> b{e.begin(), e.end()};

    WHEN("replacemerge with a non-exist node"){
      THEN("throw exception"){
        REQUIRE_THROWS_WITH(b.MergeReplace(2, 5), "Cannot call Graph::MergeReplace on old or new data if they don't exist in the graph");
      }
    }
    WHEN("replace merge with 3"){
      b.MergeReplace(2, 3);
      THEN("node 2 should be gone"){
        REQUIRE(!b.IsNode(2));
      }
      THEN("all connections belong to 2 should be 3 's"){
        std::vector<int> nodes = {3, 4};
        REQUIRE(b.GetConnected(3) == nodes);

        std::vector<std::string> w1{"A"};
        REQUIRE(b.GetWeights(1, 3) == w1);


        std::vector<std::string> w2{"B"};
        REQUIRE(b.GetWeights(3, 3) == w2);

        std::vector<std::string> w3{"B", "C"};
        REQUIRE(b.GetWeights(3, 4) == w3);
      }
    }
  }
}
