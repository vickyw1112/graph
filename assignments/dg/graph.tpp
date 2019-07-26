/* ===== Compare functors ===== */
template <typename N, typename E>
struct gdwg::Graph<N,E>::UniquePointerNodeCompare {
  bool operator()(const std::unique_ptr<N> &lhs,
                  const std::unique_ptr<N> &rhs) const {
    return *lhs < *rhs;
  }
  /* for set transparent comparison */
  bool operator()(const N &lhs, const std::unique_ptr<N> &rhs) const {
    return lhs < *rhs;
  }
  bool operator()(const std::unique_ptr<N> &lhs, const N &rhs) const {
    return *lhs < rhs;
  }
  using is_transparent = const N &;
};

template <typename N, typename E>
struct gdwg::Graph<N,E>::PointerNodeCompare {
  bool operator()(const N *lhs, const N *rhs) const { return *lhs < *rhs; }
};

template <typename N, typename E>
struct gdwg::Graph<N,E>::UniquePointerEdgeCompare {
  bool operator()(const std::unique_ptr<E> &lhs,
                  const std::unique_ptr<E> &rhs) const {
    return *lhs < *rhs;
  }
  /* for set transparent comparison */
  bool operator()(const E &lhs, const std::unique_ptr<E> &rhs) const {
    return lhs < *rhs;
  }
  bool operator()(const std::unique_ptr<E> &lhs, const E &rhs) const {
    return *lhs < rhs;
  }
  using is_transparent = const E &;
};

template <typename N, typename E>
struct gdwg::Graph<N,E>::ConnectionCompare {
  bool operator()(const Connection &lhs, const Connection &rhs) const {
    return (*(lhs.first) != *(rhs.first)) ? *(lhs.first) < *(rhs.first)
                                          : *(lhs.second) < *(rhs.second);
  }

  bool operator()(const Connection &lhs, const std::pair<N, E> &rhs) const {
    return (*(lhs.first) != rhs.first) ? *(lhs.first) < rhs.first
                                       : *(lhs.second) < rhs.second;
  }

  bool operator()(const std::pair<N, E> &lhs, const Connection &rhs) const {
    return (lhs.first != *(rhs.first)) ? lhs.first < *(rhs.first)
                                       : lhs.second < *(rhs.second);
  }

  /* for checking if connection to a specific node exists */
  bool operator()(const Connection &lhs, const N &rhs) const {
    return *(lhs.first) < rhs;
  }

  bool operator()(const N &lhs, const Connection &rhs) const {
    return lhs < *(rhs.first);
  }

  using is_transparent = const N &;
};

// ====================================CONSTRUCTOR================================================

template <typename N, typename E>
gdwg::Graph<N, E>::Graph(typename std::vector<N>::const_iterator start,
                         typename std::vector<N>::const_iterator end) {
  for (auto i = start; i != end; i++) {
    if (!IsNode(*i)) {
      std::unique_ptr<N> newNode = std::make_unique<N>(*i);
      connections_[newNode.get()] = {};
      nodes_.insert(std::move(newNode));
    }
  }
}

template <typename N, typename E>
gdwg::Graph<N, E>::Graph(typename std::vector<Edge>::const_iterator start,
                         typename std::vector<Edge>::const_iterator end) {
  for (auto i = start; i != end; i++) {
    auto &[from, to, edge] = *i;
    N *fromNode = GetNode(from);
    N *toNode = GetNode(to);
    this->InsertEdge(*fromNode, *toNode, edge);
  }
}

// init list
template <typename N, typename E>
gdwg::Graph<N, E>::Graph(std::initializer_list<N> list) {
  for (auto i = list.begin(); i != list.end(); i++) {
    if (!IsNode(*i)) {
      std::unique_ptr<N> newNode = std::make_unique<N>(*i);
      connections_[newNode.get()] = {};
      nodes_.insert(std::move(newNode));
    }
  }
}

// copy constructor
template <typename N, typename E>
gdwg::Graph<N, E>::Graph(const gdwg::Graph<N, E> &old) {
  for (const auto &pair : old.connections_) {
    auto &[from, connections] = pair;

    // make sure that node is unique
    N *fromNode = GetNode(*from);

    for (const auto &connectionPair : connections) {
      // get toNode and edge
      auto [toPtr, edgePtr] = connectionPair;

      std::unique_ptr<E> edge = std::make_unique<E>(*edgePtr);

      // make sure that node is unique
      N *toNode = GetNode(*toPtr);

      // insert connections
      this->connections_[fromNode].insert(std::make_pair(toNode, edge.get()));

      // make a record for edge
      this->edges_.insert(std::move(edge));
    }
  }
}

// move constructor
template <typename N, typename E>
gdwg::Graph<N, E>::Graph(const gdwg::Graph<N, E> &&old) {
  this->nodes_ = std::move(old.nodes_);
  this->edges_ = std::move(old.edges_);
  this->connections_ = std::move(old.connections_);
}

// ====================================METHOD================================================
template <typename N, typename E>
bool gdwg::Graph<N, E>::InsertNode(const N &node) {
  if (IsNode(node))
    return false;

  std::unique_ptr<N> newNode = std::make_unique<N>();
  *newNode = node;
  connections_[newNode.get()] = {};
  nodes_.insert(std::move(newNode));
  return true;
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::InsertEdge(const N &src, const N &dst, const E &w) {
  if (!IsNode(src) || !IsNode(dst)) {
    throw std::runtime_error("Cannot call Graph::InsertEdge when either src or "
                             "dst node does not exist");
  }

  N *from = GetNode(src);
  N *to = GetNode(dst);

  auto connections = this->connections_[from];
  // exact edge exists
  if (connections.find(std::make_pair(dst, w)) != connections.end()) {
    return false;
  }

  std::unique_ptr<E> edge = std::make_unique<E>(w);
  this->connections_[from].insert(std::make_pair(to, edge.get()));
  this->edges_.insert(std::move(edge));
  return true;
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::IsConnected(const N &src, const N &dst) const {
  if (!IsNode(src) || !IsNode(dst)) {
    throw std::runtime_error("Cannot call Graph::IsConnected if src or dst "
                             "node don't exist in the graph");
  }

  /* cannot use GetNode since it's not const */
  // N *from = GetNode(src);
  N *from = (*nodes_.find(src)).get();

  auto connections = this->connections_.at(from);
  return connections.find(dst) != connections.end();
}

template <typename N, typename E>
std::vector<E> gdwg::Graph<N, E>::GetWeights(const N &src, const N &dst) const {
  if (!IsNode(src) || !IsNode(dst)) {
    throw std::out_of_range("Cannot call Graph::GetWeights if src or dst node "
                            "don't exist in the graph");
  }

  N *from = GetNode(src);
  N *to = GetNode(dst);

  std::vector<E> res;
  auto list = this->connections_[from];
  for (const auto &pair : list) {
    auto &[currTo, edgePtr] = pair;
    if (currTo == to) {
      res.push_back(*(edgePtr));
    }
  }

  return res;
}

template <typename N, typename E>
std::vector<N> gdwg::Graph<N, E>::GetConnected(const N &src) const {
  if (!IsNode(src)) {
    throw std::out_of_range(
        "Cannot call Graph::GetConnected if src doesn't exist in the graph");
  }

  N *from = GetNode(src);
  std::vector<N> res;

  for (const auto &pair : connections_[from]) {
    res.push_back(*(pair.second));
  }

  return res;
}

template <typename N, typename E>
std::vector<N> gdwg::Graph<N, E>::GetNodes() const {
  std::vector<N> res;
  for (const auto &n : nodes_) {
    res.push_back(*(n.get()));
  }

  return res;
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::DeleteNode(const N &node) {
  if (!IsNode(node)) {
    return false;
  }
  N *deleteNodePtr = GetNode(node);
  auto uni_node = nodes_.find(node);

  // if node is a src node
  auto connection_list = connections_[deleteNodePtr];
  if (!connection_list.empty()) {
    // delete all the connection when node is src node
    for (const auto &pair : connection_list) {
      auto edge = edges_.find(*(pair.second));
      edges_.erase(edge);
    }
    // delete the entry
    connections_.erase(deleteNodePtr);
  }

  // loop through all the connections
  for (auto mapIt = connections_.begin(); mapIt != connections_.end();
       mapIt++) {
    for (auto connIt = mapIt->second.begin(); connIt != mapIt->second.end();
         connIt++) {
      auto [nodePtr, edgePtr] = *connIt;
      if (nodePtr == deleteNodePtr) {
        auto edge = edges_.find(*edgePtr);
        edges_.erase(edge);
        mapIt->second.erase(*connIt);
      }
    }
  }

  nodes_.erase(uni_node);
  return true;
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::Replace(const N &oldData, const N &newData) {
  if (!IsNode(oldData)) {
    throw std::runtime_error(
        "Cannot call Graph::Replace on a node that doesn't exist");
  }

  if (IsNode(newData)) {
    return false;
  }

  N *old = GetNode(oldData);
  *old = newData;
  return true;
}
