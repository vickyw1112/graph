#ifndef ASSIGNMENTS_DG_GRAPH_H_
#define ASSIGNMENTS_DG_GRAPH_H_

#include <iterator>
#include <map>
#include <iostream>
#include <memory>
#include <set>
#include <tuple>
#include <utility>
#include <vector>

namespace gdwg {

template <typename N, typename E>
class Graph {
 private:
  /**
   * Comapre functors
   */
  struct UniquePointerNodeCompare;
  struct PointerNodeCompare;
  struct ConnectionCompare;

  /* Each connection represent an edge from certain node to this node with this edge value
   * see below connections_ map */
  using Connection = std::pair<N*, std::unique_ptr<E>>;

 public:
  using Edge = std::tuple<N, N, E>;

  class const_iterator {
   private:
    using MapItType = typename std::
        map<N*, std::set<Connection, ConnectionCompare>, PointerNodeCompare>::const_iterator;
    using ConnectionItType = typename std::set<Connection, ConnectionCompare>::const_iterator;

    /* current iterator position for connections_ map */
    MapItType map_it_;
    /* Sentinel value for map_it_, which should be the end iterator */
    MapItType sentinel_;
    /* current iterator position for the connections set which is
     * value of connections_ map */
    ConnectionItType connection_it_;

    friend class Graph<N, E>;

    const_iterator(const MapItType& map_it,
                   const MapItType& sentinel,
                   const ConnectionItType& connection_it)
      : map_it_{map_it}, sentinel_{sentinel}, connection_it_{connection_it} {};

   public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = Edge;
    using reference = std::tuple<const N&, const N&, const E&>;
    using difference_type = int;
    using pointer = value_type*;

    reference operator*();
    pointer operator->() { return *this; }
    const_iterator operator++();
    const_iterator operator++(int);
    const_iterator operator--();
    const_iterator operator--(int);

    friend bool operator==(const const_iterator& lhs, const const_iterator& rhs) {
      return lhs.map_it_ == rhs.map_it_ &&
             (lhs.map_it_ == lhs.sentinel_ || lhs.connection_it_ == rhs.connection_it_);
    }
    friend bool operator!=(const const_iterator& lhs, const const_iterator& rhs) {
      return !(lhs == rhs);
    }
  };

  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  /* ===== Constructors ===== */
  Graph() = default;

  /* node iterator constructor */
  Graph(typename std::vector<N>::const_iterator start, typename std::vector<N>::const_iterator end);

  /* edge iterator constructor */
  Graph(typename std::vector<Edge>::const_iterator start,
        typename std::vector<Edge>::const_iterator end);

  /* node initializer_list constructor */
  Graph(std::initializer_list<N> list);

  /* copy constructor */
  explicit Graph(const gdwg::Graph<N, E>& old);

  /* move constructor */
  explicit Graph(gdwg::Graph<N, E>&& old) = default;

  ~Graph() = default;

  /* ===== Methods ===== */
  // TODO check const correctness
  bool InsertNode(const N& node);
  bool InsertEdge(const N& src, const N& dst, const E& w);
  bool DeleteNode(const N& node);
  bool Replace(const N& oldData, const N& newData);
  void MergeReplace(const N& oldData, const N& newData);
  void Clear();

  bool IsNode(const N& val) const { return connections_.find(val) != connections_.end(); }
  bool IsConnected(const N& src, const N& dst) const;

  std::vector<N> GetNodes() const;
  std::vector<N> GetConnected(const N& src) const;
  std::vector<E> GetWeights(const N& src, const N& dst) const;

  const_iterator find(const N& src, const N& dst, const E& w) const;
  bool erase(const N& src, const N& dst, const E& w);
  const_iterator erase(const_iterator it);

  const_iterator cbegin() const;
  const_iterator cend() const;
  const_reverse_iterator crbegin() const { return const_reverse_iterator{cend()}; }
  const_reverse_iterator crend() const { return const_reverse_iterator{cbegin()}; }

  const_iterator begin() { return cbegin(); }
  const_iterator end() { return cend(); }
  const_reverse_iterator rbegin() { return crbegin(); }
  const_reverse_iterator rend() { return crend(); }

  /* operator overloads */
  friend std::ostream& operator<<(std::ostream& out, const Graph<N, E>& g){
    for (const auto &pair : g.connections_) {
      const auto& [fromPtr, connectionSet] = pair;
      out << *fromPtr << " (\n";
      for (const auto &connection: connectionSet) {
        const auto &[toPtr, edgeUniqPtr] = connection;
        out << "  " << *toPtr << " | " << *edgeUniqPtr << "\n";
      }
      out << ")\n";
    }
    return out;
  }
  friend bool operator==(const Graph<N, E>& l, const Graph<N, E>& r){
    if(l.GetNodes() != r.GetNodes()) {
      return false;
    }
    return std::equal(l.cbegin(), l.cend(), r.cbegin(), r.cend());
  }
  friend bool operator!=(const Graph<N, E>& l, const Graph<N, E>& r){
    return !(l == r);
  }

 private:
  /**
   * if node exists, return raw pointer to it
   * else make unique ptr, put it to node set
   * then return raw pointer
   */
  N* GetNode(const N& val);

  /**
   * Graph attributes
   */
  /* set of all unique pointer to nodes */
  std::set<std::unique_ptr<N>, UniquePointerNodeCompare> nodes_;
  /* map to represent adjacency list */
  std::map<N*, std::set<Connection, ConnectionCompare>, PointerNodeCompare> connections_;
};

}  // namespace gdwg

#include "assignments/dg/graph.tpp"

#endif  // ASSIGNMENTS_DG_GRAPH_H_
