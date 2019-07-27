#ifndef ASSIGNMENTS_DG_GRAPH_H_
#define ASSIGNMENTS_DG_GRAPH_H_

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
  struct UniquePointerEdgeCompare;
  struct PointerNodeCompare;
  struct ConnectionCompare;

  using Connection = std::pair<N*, E*>;

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

   public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = std::tuple<N, N, E>;
    using reference = const std::tuple<const N&, const N&, const E&>;
    using different_type = int;

    const_iterator(const MapItType& map_it,
                   const MapItType& sentinel,
                   const ConnectionItType& connection_it)
      : map_it_{map_it}, sentinel_{sentinel}, connection_it_{connection_it} {};

    reference operator*();
    const_iterator operator++();
    const_iterator operator++(int);

    bool operator==(const const_iterator& rhs);
    bool operator!=(const const_iterator& rhs);
  };

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
  // TODO make it default?
  explicit Graph(const gdwg::Graph<N, E>&& old);

  ~Graph() = default;

  /* ===== Methods ===== */
  // TODO check const correctness
  bool InsertNode(const N& node);
  bool InsertEdge(const N& src, const N& dst, const E& w);
  bool DeleteNode(const N& node);
  bool Replace(const N& oldData, const N& newData);
  void MergeReplace(const N& oldData, const N& newData);
  void Clear();

  bool IsNode(const N& val) const { return nodes_.find(val) != nodes_.end(); }
  bool IsConnected(const N& src, const N& dst) const;

  std::vector<N> GetNodes() const;
  std::vector<N> GetConnected(const N& src) const;
  std::vector<E> GetWeights(const N& src, const N& dst) const;

  const_iterator find(const N& src, const N& dst, const E& w);
  bool erase(const N& src, const N& dst, const E& w);
  const_iterator erase(const_iterator it);
  const_iterator cbegin();
  const_iterator cend();

 private:
  /**
   * if node exists, return raw pointer to it
   * else make unique ptr, put it to node set
   * then return raw pointer
   */
  N* GetNode(const N& val) {
    N* temp;
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

  /**
   * Graph attributes
   */

  // TODO: make nodes_ and edges_ unordered
  /* set of all unique pointer to nodes */
  std::set<std::unique_ptr<N>, UniquePointerNodeCompare> nodes_;
  /* set of all unique pointer to edges */
  std::set<std::unique_ptr<E>, UniquePointerEdgeCompare> edges_;
  /* map to represent adjacency list that uses raw pointers */
  std::map<N*, std::set<Connection, ConnectionCompare>, PointerNodeCompare> connections_;
};

}  // namespace gdwg

#include "assignments/dg/graph.tpp"

#endif  // ASSIGNMENTS_DG_GRAPH_H_
