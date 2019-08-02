#ifndef ASSIGNMENTS_DG_GRAPH_H_
#define ASSIGNMENTS_DG_GRAPH_H_

#include <iostream>
#include <iterator>
#include <map>
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

    reference operator*() noexcept;
    pointer operator->() noexcept { return *this; }
    const_iterator operator++() noexcept;
    const_iterator operator++(int) noexcept;
    const_iterator operator--() noexcept;
    const_iterator operator--(int) noexcept;

    friend bool operator==(const const_iterator& lhs, const const_iterator& rhs) noexcept {
      return lhs.map_it_ == rhs.map_it_ &&
             (lhs.map_it_ == lhs.sentinel_ || lhs.connection_it_ == rhs.connection_it_);
    }
    friend bool operator!=(const const_iterator& lhs, const const_iterator& rhs) noexcept {
      return !(lhs == rhs);
    }
  };

  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  /* ===== Constructors ===== */
  Graph() noexcept = default;

  /* node iterator constructor */
  Graph(typename std::vector<N>::const_iterator start,
        typename std::vector<N>::const_iterator end) noexcept;

  /* edge iterator constructor */
  Graph(typename std::vector<Edge>::const_iterator start,
        typename std::vector<Edge>::const_iterator end) noexcept;

  /* node initializer_list constructor */
  Graph(std::initializer_list<N> list) noexcept;

  /* copy constructor */
  explicit Graph(const Graph<N, E>& old) noexcept;

  /* move constructor */
  explicit Graph(Graph<N, E>&& old) noexcept = default;

  ~Graph() noexcept = default;

  /* ===== Operator ====== */
  Graph<N, E>& operator=(const Graph<N, E>& old) noexcept;

  Graph<N, E>& operator=(Graph<N, E>&& old) noexcept = default;

  /* ===== Methods ===== */
  bool InsertNode(const N& node) noexcept;
  bool InsertEdge(const N& src, const N& dst, const E& w);
  bool DeleteNode(const N& node) noexcept;
  bool Replace(const N& oldData, const N& newData);
  void MergeReplace(const N& oldData, const N& newData);
  void Clear() noexcept;

  bool IsNode(const N& val) const noexcept { return connections_.find(val) != connections_.end(); }
  bool IsConnected(const N& src, const N& dst) const;

  std::vector<N> GetNodes() const noexcept;
  std::vector<N> GetConnected(const N& src) const;
  std::vector<E> GetWeights(const N& src, const N& dst) const;

  const_iterator find(const N& src, const N& dst, const E& w) const noexcept;
  bool erase(const N& src, const N& dst, const E& w) noexcept;
  const_iterator erase(const_iterator it) noexcept;

  const_iterator cbegin() const noexcept;
  const_iterator cend() const noexcept;
  const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator{cend()}; }
  const_reverse_iterator crend() const noexcept { return const_reverse_iterator{cbegin()}; }

  const_iterator begin() noexcept { return cbegin(); }
  const_iterator end() noexcept { return cend(); }
  const_reverse_iterator rbegin() noexcept { return crbegin(); }
  const_reverse_iterator rend() noexcept { return crend(); }

  /* operator overloads */
  friend std::ostream& operator<<(std::ostream& out, const Graph<N, E>& g) noexcept {
    for (const auto& pair : g.connections_) {
      const auto& [fromPtr, connectionSet] = pair;
      out << *fromPtr << " (\n";
      for (const auto& connection : connectionSet) {
        const auto& [toPtr, edgeUniqPtr] = connection;
        out << "  " << *toPtr << " | " << *edgeUniqPtr << "\n";
      }
      out << ")\n";
    }
    return out;
  }

  friend bool operator==(const Graph<N, E>& l, const Graph<N, E>& r) noexcept {
    if (l.GetNodes() != r.GetNodes()) {
      return false;
    }
    return std::equal(l.cbegin(), l.cend(), r.cbegin(), r.cend());
  }

  friend bool operator!=(const Graph<N, E>& l, const Graph<N, E>& r) noexcept { return !(l == r); }

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
