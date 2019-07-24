#ifndef ASSIGNMENTS_DG_GRAPH_H_
#define ASSIGNMENTS_DG_GRAPH_H_

#include <map>
#include <memory>
#include <set>
#include <utility>
#include <vector>
#include <iostream>

namespace gdwg {

template <typename N, typename E>
class Graph {
 public:
  using Edge = std::tuple<N, N, E>;
  // ====================================CONSTRUCTOR================================================
  Graph() = default;

  // iterator constructor
  Graph(typename std::vector<N>::const_iterator start, typename std::vector<N>::const_iterator end);

  // tuple
  Graph(typename std::vector<Edge>::const_iterator start, typename std::vector<Edge>::const_iterator end);

  // init list
  Graph(std::initializer_list<N> list);

  // copy constructor
  Graph(const gdwg::Graph<N, E>& old);

  // destructor
  ~Graph() = default;
  // ====================================METHOD================================================
  bool InsertNode(const N& node);

  bool IsNode(const N& val) {
    // std::unique_ptr<N> temp = std::make_unique<N>(val);
    return nodes_.find(val) != nodes_.end();
  }

  bool InsertEdge(const N& src, const N& dst, const E& w);

  bool IsConnected(const N& src, const N& dst);

  std::vector<E> GetWeights(const N& src, const N& dst);

  std::vector<N> GetConnected(const N& src);

  std::vector<N> GetNodes();


 private:

  // if node exists, return raw pointer
  // else make unique ptr then return raw pointer
  N* getNode(const N& val){
    N* temp;
    if(!IsNode(val)){
      std::unique_ptr<N> node = std::make_unique<N>(val);
      temp = node.get();
      this->connections_[node.get()] = {};
      this->nodes_.insert(std::move(node));
    } else {
      temp = (*this->nodes_.find(val)).get();
    }

    return temp;
  }

  using Connection = std::pair<N*, E*>;

  struct UniquePointerNodeCompare {
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

  struct PointerNodeCompare {
    bool operator()(const N* lhs, const N* rhs) const {
      return *lhs < *rhs;
    }
  };

  struct ConnectionCompare{
    bool operator()(const Connection& lhs, const Connection& rhs) const {
      return (*(lhs.first) != *(rhs.first)) ?
          *(lhs.first) < *(rhs.first) : *(lhs.second) < *(rhs.second);
    }

    bool operator()(const Connection& lhs, const std::pair<N, E>& rhs) const {
      return (*(lhs.first) != rhs.first) ?
             *(lhs.first) < rhs.first : *(lhs.second) < rhs.second;
    }

    bool operator()(const std::pair<N, E>& lhs, const Connection& rhs) const {
      return (lhs.first != *(rhs.first)) ?
             lhs.first < *(rhs.first) : lhs.second < *(rhs.second);
    }

    /* for checking if connection to a specific node exists */
    bool operator()(const Connection& lhs, const N& rhs) const {
      return *(lhs.first) < rhs;
    }

    bool operator()(const N& lhs, const Connection& rhs) const {
      return lhs < *(rhs.first);
    }

    using is_transparent = const N&;
  };
  std::set<std::unique_ptr<N>, UniquePointerNodeCompare> nodes_;
  std::set<std::unique_ptr<E>> edges_;
  std::map<N*, std::set<Connection, ConnectionCompare>, PointerNodeCompare> connections_;

 public:

  class const_iterator {
   private:
    Graph& g_;
    std::set<Connection, ConnectionCompare> emptyConnection_;
    typename std::map<N*, std::set<Connection, ConnectionCompare>, PointerNodeCompare>::const_iterator map_it_;
    typename std::set<Connection, ConnectionCompare>::const_iterator connection_it_;

   public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = std::tuple<N, N, E>;
    using reference = const std::tuple<const N&, const N&, const E&>;
    using different_type = int;

    explicit const_iterator(Graph& g) : g_{g}, map_it_{g.connections_.cbegin()},
        connection_it_{map_it_ == g.connections_.cend() ? emptyConnection_.cend() : map_it_->second.cbegin()} {}

    const_iterator(Graph& g,
                   typename std::map<N*, std::set<Connection, ConnectionCompare>, PointerNodeCompare>::const_iterator map_it)
      : g_{g}, map_it_{map_it}, connection_it_{emptyConnection_.cend()} {}

    const_iterator(Graph& g,
      typename std::map<N*, std::set<Connection, ConnectionCompare>, PointerNodeCompare>::const_iterator map_it,
      typename std::set<Connection, ConnectionCompare>::const_iterator connection_it)
      : g_{g}, map_it_{map_it}, connection_it_{connection_it} {}

    reference operator*() {
      return {*(map_it_->first), *(connection_it_->first), *(connection_it_->second)};
    }

    const_iterator operator++() {
      ++connection_it_;
      while (connection_it_ == map_it_->second.cend()) {
        if (++map_it_ != g_.connections_.cend()) {
          connection_it_ = map_it_->second.cbegin();
        } else {
          return *this; // reached cend()
        }
      }
      return *this;
    }

    const_iterator operator++(int) {
      const_iterator copy = *this;
      ++*this;
      return copy;
    }

    bool operator==(const const_iterator& rhs) {
      return map_it_ == rhs.map_it_ &&
        (map_it_ == g_.connections_.cend() || connection_it_ == rhs.connection_it_);
    }

    bool operator!=(const const_iterator& rhs) {
      return !(*this == rhs);
    }
  };

  const_iterator cbegin() {
    return const_iterator{*this};
  }

  const_iterator cend() {
    return const_iterator{*this, connections_.cend()};
  }

};

}  // namespace gdwg

#include "assignments/dg/graph.tpp"

#endif  // ASSIGNMENTS_DG_GRAPH_H_
