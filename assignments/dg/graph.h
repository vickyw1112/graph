#ifndef ASSIGNMENTS_DG_GRAPH_H_
#define ASSIGNMENTS_DG_GRAPH_H_

#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <utility>
#include <vector>

namespace gdwg {

template <typename N, typename E> class Graph {
public:
  using Edge = std::tuple<N, N, E>;

  /* ===== Constructors ===== */
  Graph() = default;

  /* node iterator constructor */
  Graph(typename std::vector<N>::const_iterator start,
        typename std::vector<N>::const_iterator end);

  /* edge iterator constructor */
  Graph(typename std::vector<Edge>::const_iterator start,
        typename std::vector<Edge>::const_iterator end);

  /* node initializer_list constructor */
  Graph(std::initializer_list<N> list);

  /* copy constructor */
  Graph(const gdwg::Graph<N, E> &old);

  /* move constructor */
  // TODO make it default?
  Graph(const gdwg::Graph<N, E> &&old);

  ~Graph() = default;

  /* ===== Methods ===== */
  // TODO check const correctness
  bool InsertNode(const N &node);

  bool IsNode(const N &val) const { return nodes_.find(val) != nodes_.end(); }

  bool InsertEdge(const N &src, const N &dst, const E &w);

  bool IsConnected(const N &src, const N &dst) const;

  std::vector<E> GetWeights(const N &src, const N &dst) const;

  std::vector<N> GetConnected(const N &src) const;

  std::vector<N> GetNodes() const;

  bool DeleteNode(const N &node);

  bool Replace(const N &oldData, const N &newData);

  void MergeReplace(const N &oldData, const N &newData);

private:
  /**
   * if node exists, return raw pointer to it
   * else make unique ptr, put it to node set
   * then return raw pointer
   */
  N *GetNode(const N &val) {
    N *temp;
    if (!IsNode(val)) {
      std::unique_ptr<N> node = std::make_unique<N>(val);
      temp = node.get();
      this->connections_[node.get()] = {};
      this->nodes_.insert(std::move(node));
    } else {
      temp = (*this->nodes_.find(val)).get();
    }

    return temp;
  }

  using Connection = std::pair<N *, E *>;

  /**
   * Comapre functors
   */
  struct UniquePointerNodeCompare;
  struct PointerNodeCompare;
  struct UniquePointerEdgeCompare;
  struct ConnectionCompare;

  /**
   * Graph attributes
   */

  /* set of all unique pointer to nodes */
  std::set<std::unique_ptr<N>, UniquePointerNodeCompare> nodes_;
  /* set of all unique pointer to edges */
  std::set<std::unique_ptr<E>, UniquePointerEdgeCompare> edges_;
  /* map to represent adjacency list that uses raw pointers */
  std::map<N *, std::set<Connection, ConnectionCompare>, PointerNodeCompare>
      connections_;

public:
  class const_iterator {
  private:
    /* ref to graph */
    Graph &g_;
    /* current iterator position for connections_ map */
    typename std::map<N *, std::set<Connection, ConnectionCompare>,
                      PointerNodeCompare>::const_iterator map_it_;
    /* current iterator position for the connections set which is 
     * value of connections_ map */
    typename std::set<Connection, ConnectionCompare>::const_iterator
        connection_it_;
    /* we use cend of this empty set as a special value for connection_it_ */
    std::set<Connection, ConnectionCompare> emptyConnection_;

  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = std::tuple<N, N, E>;
    using reference = const std::tuple<const N &, const N &, const E &>;
    using different_type = int;

    explicit const_iterator(Graph &g)
        : g_{g}, map_it_{g.connections_.cbegin()},
          connection_it_{map_it_ == g.connections_.cend()
                             ? emptyConnection_.cend()
                             : map_it_->second.cbegin()} {}

    const_iterator(
        Graph &g,
        typename std::map<N *, std::set<Connection, ConnectionCompare>,
                          PointerNodeCompare>::const_iterator map_it)
        : g_{g}, map_it_{map_it}, connection_it_{emptyConnection_.cend()} {}

    const_iterator(
        Graph &g,
        typename std::map<N *, std::set<Connection, ConnectionCompare>,
                          PointerNodeCompare>::const_iterator map_it,
        typename std::set<Connection, ConnectionCompare>::const_iterator
            connection_it)
        : g_{g}, map_it_{map_it}, connection_it_{connection_it} {}

    reference operator*() {
      return {*(map_it_->first), *(connection_it_->first),
              *(connection_it_->second)};
    }

    const_iterator operator++() {
      ++connection_it_;
      while (connection_it_ == map_it_->second.cend()) {
        if (++map_it_ != g_.connections_.cend()) {
          connection_it_ = map_it_->second.cbegin();
        } else {
          return *this; /* reached cend() */
        }
      }
      return *this;
    }

    const_iterator operator++(int) {
      const_iterator copy = *this;
      ++*this;
      return copy;
    }

    bool operator==(const const_iterator &rhs) {
      return map_it_ == rhs.map_it_ && (map_it_ == g_.connections_.cend() ||
                                        connection_it_ == rhs.connection_it_);
    }

    bool operator!=(const const_iterator &rhs) { return !(*this == rhs); }
  };

  const_iterator cbegin() { return const_iterator{*this}; }

  const_iterator cend() { return const_iterator{*this, connections_.cend()}; }
};

} // namespace gdwg

#include "assignments/dg/graph.tpp"

#endif // ASSIGNMENTS_DG_GRAPH_H_
