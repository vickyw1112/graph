#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <set>
#include <utility>
#include <vector>

/* ===== Private Methods ===== */
/**
 * if node exists, return raw pointer to it
 * else make unique ptr, put it to node set
 * then return raw pointer
 */
template <typename N, typename E>
N* gdwg::Graph<N, E>::GetNode(const N& val) {
  N* temp;
  if (!IsNode(val)) {
    std::unique_ptr<N> node = std::make_unique<N>(val);
    temp = node.get();
    this->connections_[node.get()] = std::set<Connection, ConnectionCompare>{};
    this->nodes_.insert(std::move(node));
  } else {
    temp = connections_.find(val)->first;
  }

  return temp;
}

/* ===== Iterator Overloads ===== */
/* dereference Iterator */
template <typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator::reference gdwg::Graph<N, E>::const_iterator::
operator*() {
  return {*(map_it_->first), *(connection_it_->first), *(connection_it_->second)};
}

/* pre increment */
template <typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator gdwg::Graph<N, E>::const_iterator::operator++() {
  ++connection_it_;
  /* check if there is a connection in current incremented connection_it_ */
  while (connection_it_ == map_it_->second.cend()) {
    /* if no connections, go to next node */
    if (++map_it_ != sentinel_) {
      connection_it_ = map_it_->second.cbegin();
    } else {
      return *this; /* reached cend() */
    }
  }
  return *this;
}

/* post increment */
template <typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator gdwg::Graph<N, E>::const_iterator::operator++(int) {
  const_iterator copy = *this;
  ++*this;
  return copy;
}

/* pre decrement */
template <typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator gdwg::Graph<N, E>::const_iterator::operator--() {
  /* if we're at end() */
  if (map_it_ == sentinel_) {
    map_it_--;
    while (map_it_->second.cend() == map_it_->second.cbegin()) {
      map_it_--;
    }
    connection_it_ = std::prev(map_it_->second.cend());
    return *this;
  }

  /* if connection_it_ is first iterator of current connections set */
  while (connection_it_ == map_it_->second.cbegin()) {
    map_it_--;
    connection_it_ = map_it_->second.cend();
  }
  --connection_it_;
  return *this;
}

/* post decrement */
template <typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator gdwg::Graph<N, E>::const_iterator::operator--(int) {
  const_iterator copy = *this;
  --*this;
  return copy;
}

/* ===== Compare functors ===== */
template <typename N, typename E>
struct gdwg::Graph<N, E>::UniquePointerNodeCompare {
  bool operator()(const std::unique_ptr<N>& lhs, const std::unique_ptr<N>& rhs) const {
    return *lhs < *rhs;
  }
  /* for set transparent comparison */
  bool operator()(const N& lhs, const std::unique_ptr<N>& rhs) const { return lhs < *rhs; }
  bool operator()(const std::unique_ptr<N>& lhs, const N& rhs) const { return *lhs < rhs; }
  using is_transparent = void;
};

template <typename N, typename E>
struct gdwg::Graph<N, E>::PointerNodeCompare {
  bool operator()(const N* lhs, const N* rhs) const { return *lhs < *rhs; }
  /* for set transparent comparison */
  bool operator()(const N& lhs, const N* rhs) const { return lhs < *rhs; }
  bool operator()(const N* lhs, const N& rhs) const { return *lhs < rhs; }
  using is_transparent = void;
};

template <typename N, typename E>
struct gdwg::Graph<N, E>::ConnectionCompare {
  bool operator()(const Connection& lhs, const Connection& rhs) const {
    return (*(lhs.first) != *(rhs.first)) ? *(lhs.first) < *(rhs.first)
                                          : *(lhs.second) < *(rhs.second);
  }

  bool operator()(const Connection& lhs, const std::pair<N, E>& rhs) const {
    return (*(lhs.first) != rhs.first) ? *(lhs.first) < rhs.first : *(lhs.second) < rhs.second;
  }

  bool operator()(const std::pair<N, E>& lhs, const Connection& rhs) const {
    return (lhs.first != *(rhs.first)) ? lhs.first < *(rhs.first) : lhs.second < *(rhs.second);
  }

  /* for checking if connection to a specific node exists */
  bool operator()(const Connection& lhs, const N& rhs) const { return *(lhs.first) < rhs; }

  bool operator()(const N& lhs, const Connection& rhs) const { return lhs < *(rhs.first); }

  using is_transparent = void;
};

/* ===== CONSTRUCTOR ===== */

template <typename N, typename E>
gdwg::Graph<N, E>::Graph(typename std::vector<N>::const_iterator start,
                         typename std::vector<N>::const_iterator end) {
  for (auto i = start; i != end; i++) {
    if (!IsNode(*i)) {
      std::unique_ptr<N> newNode = std::make_unique<N>(*i);
      connections_[newNode.get()] = std::set<Connection, ConnectionCompare>{};
      nodes_.insert(std::move(newNode));
    }
  }
}

template <typename N, typename E>
gdwg::Graph<N, E>::Graph(typename std::vector<Edge>::const_iterator start,
                         typename std::vector<Edge>::const_iterator end) {
  for (auto i = start; i != end; i++) {
    const auto& [from, to, edge] = *i;
    N* fromNode = GetNode(from);
    N* toNode = GetNode(to);
    this->InsertEdge(*fromNode, *toNode, edge);
  }
}

template <typename N, typename E>
gdwg::Graph<N, E>::Graph(std::initializer_list<N> list) {
  for (auto i = list.begin(); i != list.end(); i++) {
    if (!IsNode(*i)) {
      std::unique_ptr<N> newNode = std::make_unique<N>(*i);
      connections_[newNode.get()] = std::set<Connection, ConnectionCompare>{};
      nodes_.insert(std::move(newNode));
    }
  }
}

template <typename N, typename E>
gdwg::Graph<N, E>::Graph(const gdwg::Graph<N, E>& old) {
  for (const auto& pair : old.connections_) {
    const auto& [from, connections] = pair;

    /* make sure that node is unique */
    N* fromNode = GetNode(*from);

    for (const auto& connectionPair : connections) {
      const auto& [toPtr, edgePtr] = connectionPair;

      N* toNode = GetNode(*toPtr);

      /* insert connections */
      this->connections_[fromNode].insert(std::make_pair(toNode, std::make_unique<E>(*edgePtr)));
    }
  }
}

/* ===== METHOD ====== */
template <typename N, typename E>
bool gdwg::Graph<N, E>::InsertNode(const N& node) {
  if (IsNode(node))
    return false;

  std::unique_ptr<N> newNode = std::make_unique<N>(node);
  connections_[newNode.get()] = std::set<Connection, ConnectionCompare>{};
  nodes_.insert(std::move(newNode));
  return true;
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::InsertEdge(const N& src, const N& dst, const E& w) {
  if (!IsNode(src) || !IsNode(dst)) {
    throw std::runtime_error("Cannot call Graph::InsertEdge when either src or "
                             "dst node does not exist");
  }

  N* from = GetNode(src);
  N* to = GetNode(dst);

  const auto& connections = this->connections_[from];
  /* exact same edge exists */
  if (connections.find(std::make_pair(dst, w)) != connections.end()) {
    return false;
  }

  /* create edge and add it to graph */
  this->connections_[from].insert(std::make_pair(to, std::make_unique<E>(w)));
  return true;
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::DeleteNode(const N& node) {
  if (!IsNode(node)) {
    return false;
  }
  N* deleteNodePtr = GetNode(node);

  /* delete the map entry for all edges from delete node */
  connections_.erase(deleteNodePtr);

  /* loop through all the connections to delete edges connected to this node */
  for (auto mapIt = connections_.begin(); mapIt != connections_.end(); mapIt++) {
    for (auto connIt = mapIt->second.begin(); connIt != mapIt->second.end();) {
      const auto& [nodePtr, edgePtr] = *connIt;
      /* delete edge if dst node is the node we're deleting */
      if (nodePtr == deleteNodePtr) {
        connIt = mapIt->second.erase(connIt); /* connIt is invalidated */
      } else {
        connIt++;
      }
    }
  }

  /* remove node unique ptr from nodes_, thus free node */
  auto it = nodes_.find(node);
  nodes_.erase(it);
  return true;
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::Replace(const N& oldData, const N& newData) {
  if (!IsNode(oldData)) {
    throw std::runtime_error("Cannot call Graph::Replace on a node that doesn't exist");
  }

  if (IsNode(newData)) {
    return false;
  }

  N* old = GetNode(oldData);
  *old = newData;
  return true;
}

// TODO: fix this, have to traverse all edges
template <typename N, typename E>
void gdwg::Graph<N, E>::MergeReplace(const N& oldData, const N& newData) {
  if (!IsNode(oldData) || !IsNode(newData)) {
    throw std::runtime_error(
        "Cannot call Graph::MergeReplace on old or new data if they don't exist in the graph");
  }

  N* old = getNode(oldData);
  N* newN = getNode(newData);

  // if oldnode is a src node
  auto& connection_list = connections_[old];
  auto& newConnectList = connections_[newN];

  for (auto pairIt = newConnectList.begin(); pairIt != newConnectList.end();) {
    // change all oldnode to new node
    // make sure no dup value
    if (pairIt->first == old) {
      /* check if have dup pair */
      auto newPair = std::make_pair(newN, pairIt->second);

      /* newPair is different with pairs in newConnections */
      if (newConnectList.find(newPair) == newConnectList.end()) {
        newConnectList.insert(newPair);
      }
      // delete old pair
      pairIt = newConnectList.erase(pairIt);
    } else {
      pairIt++;
    }
  }

  // merge all connections which belong to oldNode to newNode
  // delete all the connection when node is src node
  for (const auto& pair : connection_list) {
    /* change oldNode to newNode */
    if (pair.first == old) {
      pair.first = newN;
    }

    if (newConnectList.find(pair) == newConnectList.end()) {
      newConnectList.insert(pair);
    }
  }
  // delete the entry
  connections_.erase(old);

  // delete the old node after merge
  auto deleteIt = nodes_.find(oldData);
  nodes_.erase(deleteIt);
}

template <typename N, typename E>
void gdwg::Graph<N, E>::Clear() {
  connections_.clear();
  nodes_.clear();
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::IsConnected(const N& src, const N& dst) const {
  if (!IsNode(src) || !IsNode(dst)) {
    throw std::runtime_error("Cannot call Graph::IsConnected if src or dst "
                             "node don't exist in the graph");
  }

  /* cannot use GetNode since it's not const */
  N* from = connections_.find(src)->first;

  auto& connections = this->connections_.at(from);
  return connections.find(dst) != connections.end();
}

template <typename N, typename E>
std::vector<N> gdwg::Graph<N, E>::GetNodes() const {
  std::vector<N> res;
  auto outputIt = std::back_inserter(res);
  auto copyFn = [](const auto& p) { return *(p.first); };
  transform(connections_.cbegin(), connections_.cend(), outputIt, copyFn);
  return res;
}

template <typename N, typename E>
std::vector<N> gdwg::Graph<N, E>::GetConnected(const N& src) const {
  if (!IsNode(src)) {
    throw std::out_of_range("Cannot call Graph::GetConnected if src doesn't exist in the graph");
  }

  N* from = GetNode(src);
  std::vector<N> res;

  for (const auto& pair : connections_[from]) {
    res.push_back(*(pair.second));
  }

  return res;
}

template <typename N, typename E>
std::vector<E> gdwg::Graph<N, E>::GetWeights(const N& src, const N& dst) const {
  if (!IsNode(src) || !IsNode(dst)) {
    throw std::out_of_range("Cannot call Graph::GetWeights if src or dst node "
                            "don't exist in the graph");
  }

  N* from = GetNode(src);
  N* to = GetNode(dst);

  std::vector<E> res;
  auto list = this->connections_[from];
  for (const auto& pair : list) {
    const auto& [currTo, edgePtr] = pair;
    if (currTo == to) {
      res.push_back(*(edgePtr));
    }
  }

  return res;
}

template <typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator
gdwg::Graph<N, E>::find(const N& src, const N& dst, const E& w) const {
  auto mapIt = connections_.find(src);
  if (mapIt == connections_.end())
    return cend();

  /* get connections set of src node */
  auto& connections = mapIt->second;

  auto connIt = connections.find(std::make_pair(dst, w));
  if (connIt == connections.end()) {
    return cend();
  } else {
    return const_iterator{mapIt, connections_.end(), connIt};
  }
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::erase(const N& src, const N& dst, const E& w) {
  const_iterator it = find(src, dst, w);
  if (it == cend())
    return false;
  erase(it);
  return true;
}

template <typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator
gdwg::Graph<N, E>::erase(typename gdwg::Graph<N, E>::const_iterator it) {
  if (it == cend())
    return cend();

  /* have to use .at to get non-const connections set */
  auto& connections = connections_.at(it.map_it_->first);
  /* cast result to const iterator */
  decltype(it.connection_it_) nextConn = connections.erase(it.connection_it_);

  /* handle if nextConn is end of current connections set */
  while (nextConn == it.map_it_->second.cend()) {
    if ((++it.map_it_) == connections_.cend()) {
      return cend();
    }
    nextConn = it.map_it_->second.begin();
  }

  return const_iterator{it.map_it_, connections_.cend(), nextConn};
}

template <typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator gdwg::Graph<N, E>::cbegin() const {
  auto mapIt = connections_.cbegin();
  if (connections_.cbegin() == connections_.cend()) {
    return cend();
  }

  /* ignore leading empty connectioon nodes */
  while (mapIt->second.cbegin() == mapIt->second.cend()) {
    if (++mapIt == connections_.cend())
      return cend();
  }

  return {mapIt, connections_.cend(), mapIt->second.cbegin()};
}

template <typename N, typename E>
typename gdwg::Graph<N, E>::const_iterator gdwg::Graph<N, E>::cend() const {
  return {connections_.cend(), connections_.cend(), {}};
}
