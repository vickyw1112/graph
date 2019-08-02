/*

  == Explanation and rational of testing ==

  Explain and justify how you approached testing, the degree
   to which you're certain you have covered all possibilities,
   and why you think your tests are that thorough.

*/

#include <algorithm>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include "assignments/dg/graph.h"
#include "catch.h"

std::random_device rd;
std::mt19937 random_engine(rd());

SCENARIO("Operator tests") {
  GIVEN("Graph<int, int> of {1, 1, 1}, {1, 1, 2}, {2, -, -}  {3, 1, 0}") {
    std::vector<std::tuple<int, int, int>> edges{{1, 1, 1}, {1, 1, 2}, {3, 1, 0}};
    gdwg::Graph<int, int> g{edges.begin(), edges.end()};
    g.InsertNode(2);
    WHEN("Calling output stream iterator") {
      std::stringstream ss;
      ss << g;
      const std::string output{"1 (\n"
                               "  1 | 1\n"
                               "  1 | 2\n"
                               ")\n"
                               "2 (\n"
                               ")\n"
                               "3 (\n"
                               "  1 | 0\n"
                               ")\n"};
      REQUIRE(ss.str() == output);
    }
    WHEN("Compare g to itself") {
      THEN("They should be equal") { REQUIRE(g == g); }
    }
    WHEN("Compare g to a copy of it") {
      gdwg::Graph<int, int> g2{g};
      THEN("They should be equal") {
        REQUIRE(g == g2);
        REQUIRE(!(g != g2));
      }
    }
    WHEN("Compare g to a modified copy (delete node 2) of it") {
      gdwg::Graph<int, int> g2{g};
      g2.DeleteNode(2);
      THEN("They should be not equal") {
        REQUIRE(g != g2);
        REQUIRE(!(g == g2));
      }
    }
    WHEN("Compare g to a modified copy (delete edge {1, 1, 1}) of it") {
      gdwg::Graph<int, int> g2{g};
      REQUIRE(g2.erase(1, 1, 1));
      THEN("They should be not equal") {
        REQUIRE(g != g2);
        REQUIRE(!(g == g2));
      }
    }
    WHEN("Compare g to a modified copy (add edge {1, 1, 0}) of it") {
      gdwg::Graph<int, int> g2{g};
      REQUIRE(g2.InsertEdge(1, 1, 0));
      THEN("They should be not equal") {
        REQUIRE(g != g2);
        REQUIRE(!(g == g2));
      }
    }
    WHEN("Compare g to an empty graph") {
      gdwg::Graph<int, int> g2;
      THEN("They should be not equal") {
        REQUIRE(g != g2);
        REQUIRE(!(g == g2));
      }
    }
  }
  GIVEN("empty Graph<int, int> g1, g2") {
    const gdwg::Graph<int, int> g1, g2;
    WHEN("Compare g1 and g2") {
      THEN("They should be equal") {
        REQUIRE(g1 == g2);
        REQUIRE(!(g1 != g2));
      }
    }
    THEN("Using output stream op") {
      std::stringstream ss;
      ss << g1;
      THEN("Should give empty string") { REQUIRE(ss.str().size() == 0); }
    }
  }
}

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
      THEN("Should be the same as cend()") { REQUIRE(g.cbegin() == g.cend()); }
    }
    WHEN("finding edge 1 1 1") {
      THEN("Should return cend()") { REQUIRE(g.find(1, 1, 1) == g.cend()); }
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
    gdwg::Graph<std::string, double> copyG{b};
    std::vector<std::string> s{"hello", "how", "are"};
    sort(s.begin(), s.end());
    WHEN("copy this graph") {
      THEN("has all nodes") {
        REQUIRE(copyG.GetNodes() == s);
        const std::vector<double> w1{5.4};
        REQUIRE(copyG.GetWeights("hello", "how") == w1);
        const std::vector<double> w2{7.6};
        REQUIRE(copyG.GetWeights("how", "are") == w2);
      }

      WHEN("deletea node in copyG") {
        copyG.DeleteNode("hello");
        std::vector<std::string> nodes{"are", "how"};
        THEN("original graph should have no changes") {
          REQUIRE(copyG.GetNodes() == nodes);
          REQUIRE(b.GetNodes() == s);

          std::vector<double> w1{5.4};
          REQUIRE_THROWS_WITH(
              copyG.GetWeights("hello", "how"),
              "Cannot call Graph::GetWeights if src or dst node don't exist in the graph");
          const std::vector<double> w2{7.6};
          REQUIRE(copyG.GetWeights("how", "are") == w2);

          REQUIRE(b.GetWeights("hello", "how") == w1);
        }
      }
    }
  }
}

SCENARIO("test insert edge method") {
  GIVEN("a simple graph") {
    int s1 = 1;
    int s2 = 2;
    int s3 = 3;
    auto e1 = std::make_tuple(s1, s2, "A");
    auto e2 = std::make_tuple(s2, s3, "B");
    auto e = std::vector<std::tuple<int, int, std::string>>{e1, e2};
    gdwg::Graph<int, std::string> b{e.begin(), e.end()};
    WHEN("insert nodes") {
      b.InsertNode(4);
      b.InsertNode(5);
      THEN("4 , 5 are nodes") {
        REQUIRE(b.IsNode(4));
        REQUIRE(b.IsNode(5));
      }
    }
    WHEN("insert edge between an unexist src and dst node") {
      REQUIRE_THROWS_WITH(
          b.InsertEdge(4, 5, "A"),
          "Cannot call Graph::InsertEdge when either src or dst node does not exist");
    }
    WHEN("insert edge A between 1 and 2") {
      THEN("fail - dup edge") { REQUIRE(!b.InsertEdge(1, 2, "A")); }
    }
    WHEN("insert edge B between 1, 2") {
      THEN("success") { REQUIRE(b.InsertEdge(1, 2, "B")); }
    }
  }
}

SCENARIO("test delete node method") {
  GIVEN("a simple graph") {
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
    WHEN("delete non-exist node") {
      THEN("false") { REQUIRE(!b.DeleteNode(5)); }
    }
    WHEN("delete node 2") {
      REQUIRE(b.DeleteNode(2));
      THEN("connections about node 2 should be removed") {
        REQUIRE(!b.IsNode(2));
        REQUIRE(b.GetConnected(1).empty());
        REQUIRE_THROWS_WITH(
            b.GetWeights(2, 3),
            "Cannot call Graph::GetWeights if src or dst node don't exist in the graph");
        REQUIRE_THROWS_WITH(b.GetConnected(2),
                            "Cannot call Graph::GetConnected if src doesn't exist in the graph");
      }
    }
  }
}

SCENARIO("test replace method") {
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
  WHEN("Replace a non-exist node") {
    THEN("throw exception") {
      REQUIRE_THROWS_WITH(b.Replace(5, 4),
                          "Cannot call Graph::Replace on a node that doesn't exist");
    }
  }
  WHEN("replace node 2 with 4") { REQUIRE(!b.Replace(2, 4)); }

  WHEN("replace node 2 with 5") {
    REQUIRE(b.Replace(2, 5));
    THEN("node 2 becomes 5") {
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

SCENARIO("test mergereplace method") {
  GIVEN("a simple graph") {
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

    WHEN("replacemerge with a non-exist node") {
      THEN("throw exception") {
        REQUIRE_THROWS_WITH(
            b.MergeReplace(2, 5),
            "Cannot call Graph::MergeReplace on old or new data if they don't exist in the graph");
      }
    }
    WHEN("replace merge with 3") {
      b.MergeReplace(2, 3);
      THEN("node 2 should be gone") { REQUIRE(!b.IsNode(2)); }
      THEN("all connections belong to 2 should be 3 's") {
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
    WHEN("Merge replace 2 with 2") {
      gdwg::Graph<int, std::string> copy{b};
      b.MergeReplace(2, 2);
      THEN("Graph should be the same") { REQUIRE(b == copy); }
    }
  }
}
