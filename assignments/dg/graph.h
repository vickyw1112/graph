#ifndef ASSIGNMENTS_DG_GRAPH_H_
#define ASSIGNMENTS_DG_GRAPH_H_

#include <map>
#include <memory>
#include <set>
#include <utility>
#include <list>

namespace gdwg {

template <typename N, typename E>
class Graph {
 public:
  Graph() = default;

  bool InsertNode(const N& node);

  bool IsNode(const N& val) {
    // std::unique_ptr<N> temp = std::make_unique<N>(val);
    return nodes_.find(val) != nodes_.end();
  }

  class const_iterator {};

 private:
  struct NodeCompare {
    bool operator()(const std::unique_ptr<N>& lhs, const std::unique_ptr<N>& rhs) const {
        return *lhs < *rhs;
    }
    /* for set transparent comparison */
    bool operator()(const N& lhs, const std::unique_ptr<N>& rhs) const {
        return lhs < *rhs;
    }
    bool operator()(const std::unique_ptr<N>& lhs, const N& rhs) const {
        return *lhs < rhs;
    }
    using is_transparent = const N&;
  };

  using Connection = std::pair<N*, E*>;
  std::set<std::unique_ptr<N>, NodeCompare> nodes_;
  std::set<std::unique_ptr<E>> edges_;
  std::map<N*, std::list<Connection>> connections_;

};

}  // namespace gdwg

#include "assignments/dg/graph.tpp"

#endif  // ASSIGNMENTS_DG_GRAPH_H_
