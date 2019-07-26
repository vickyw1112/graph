// ====================================CONSTRUCTOR================================================

template <typename N, typename E>
gdwg::Graph<N, E>::Graph(typename std::vector<N>::const_iterator start, typename std::vector<N>::const_iterator end){
    for(auto i = start; i != end; i++){
      if(!IsNode(*i)) {
        std::unique_ptr<N> newNode = std::make_unique<N>(*i);
        connections_[newNode.get()] = {};
        nodes_.insert(std::move(newNode));
      }
    }
}

template <typename N, typename E>
gdwg::Graph<N, E>::Graph(typename std::vector<Edge>::const_iterator start, typename std::vector<Edge>::const_iterator end){
    for(auto i = start; i != end; i++){
        // make nodes and edge

        //std::unique_ptr<N> preNode = std::make_unique<N>(std::get<0>(*i));
        //std::unique_ptr<N> afterNode = std::make_unique<N>(std::get<1>(*i));

        N* fromNode = getNode(std::get<0>(*i));
        N* toNode = getNode(std::get<1>(*i));
        //std::unique_ptr<E> edge = std::make_unique<E>(std::get<2>(*i));

      // make connections
        /*Connection connection;
        connection.first = afterNode.get();
        connection.second = edge.get();
        */

        this->InsertEdge(*fromNode, *toNode, std::get<2>(*i));
    }
}

// init list
template <typename N, typename E>
gdwg::Graph<N, E>::Graph(std::initializer_list<N> list){
    for(auto i = list.begin(); i != list.end(); i++){
        if(!IsNode(*i)) {
          std::unique_ptr<N> newNode = std::make_unique<N>(*i);
          connections_[newNode.get()] = {};
          nodes_.insert(std::move(newNode));
        }
    }
}

// copy constructor
template <typename N, typename E>
gdwg::Graph<N, E>::Graph(const gdwg::Graph<N, E>& old){
  for(const auto& pair : old.connections_){
    auto from = pair.first;

    // make sure that node is unique
    N* fromNode = getNode(*from);

    auto connections = pair.second;
    for(const auto& tuple: connections){
      // get toNode and weight
      auto toTemp = tuple.first;
      std::unique_ptr<E> weight = std::make_unique<E>(*(tuple.second));

      // make sure that node is unique
      N* toNode = getNode(*toTemp);

      // insert connections
      this->connections_[fromNode].insert(std::make_pair(toNode, weight.get()));

      // make a record for edge
      this->edges_.insert(std::move(weight));
    }
  }
}

// move constructor
template <typename N, typename E>
gdwg::Graph<N, E>::Graph(const gdwg::Graph<N, E>&& old){
  this->nodes_ = std::move(old.nodes_);
  this->edges_ = std::move(old.edges_);
  this->connections_ = std::move(old.connections_);
}

// ====================================METHOD================================================
template <typename N, typename E>
bool gdwg::Graph<N, E>::InsertNode(const N& node) {
    if (IsNode(node))
      return false;

    std::unique_ptr<N> newNode = std::make_unique<N>();
    *newNode = node;
    connections_[newNode.get()] = {};
    nodes_.insert(std::move(newNode));
    return true;
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::InsertEdge(const N& src, const N& dst, const E& w) {
  if (!IsNode(src) || !IsNode(dst)) {
    throw std::runtime_error(
      "Cannot call Graph::InsertEdge when either src or dst node does not exist");
  }

  N* from = getNode(src);
  N* to = getNode(dst);

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
bool gdwg::Graph<N, E>::IsConnected(const N& src, const N& dst){
  if(!IsNode(src) || !IsNode(dst)){
    throw std::runtime_error("Cannot call Graph::IsConnected if src or dst node don't exist in the graph");
  }

  N* from = getNode(src);

  auto connections = this->connections_[from];
  return connections.find(dst) != connections.end();
}

template <typename N, typename E>
std::vector<E> gdwg::Graph<N, E>::GetWeights(const N& src, const N& dst){
  if(!IsNode(src) || !IsNode(dst)){
    throw std::out_of_range("Cannot call Graph::GetWeights if src or dst node don't exist in the graph");
  }

  N* from = getNode(src);
  N* to = getNode(dst);

  std::vector<E> res;
  auto list = this->connections_[from];
  for(const auto& pair : list){
    if(pair.first == to){
      res.push_back(*(pair.second));
    }
  }

  return res;
}

template <typename N, typename E>
std::vector<N> gdwg::Graph<N, E>::GetConnected(const N& src){
  if(!IsNode(src)){
    throw std::out_of_range( "Cannot call Graph::GetConnected if src doesn't exist in the graph");
  }

  N* from = getNode(src);
  std::vector<N> res;

  for(const auto& pair : connections_[from]){
    res.push_back(*(pair.second));
  }

  return res;
}

template <typename N, typename E>
std::vector<N> gdwg::Graph<N, E>::GetNodes(){
  std::vector<N> res;
  for(const auto& n : nodes_){
    res.push_back(*(n.get()));
  }

  return res;
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::DeleteNode(const N& node) {
  if (!IsNode(node)) {
    return false;
  }
  N* deleted = getNode(node);
  auto uni_node = nodes_.find(node);

  // if node is a src node
  auto connection_list = connections_[deleted];
  if(!connection_list.empty()){
    // delete all the connection when node is src node
    for(const auto& pair : connection_list){
      auto edge = edges_.find(*(pair.second));
      edges_.erase(edge);
    }
    // delete the entry
    connections_.erase(deleted);
  }

  // loop through all the connections
  for(auto i = connections_.begin(); i != connections_.end(); i++){
    for(auto pair = (*i).second.begin(); pair != (*i).second.end(); pair++){
      if((*pair).first == deleted){
        auto edge = edges_.find(*((*pair).second));
        edges_.erase(edge);
        (*i).second.erase(*pair);
      }
    }
  }

  nodes_.erase(uni_node);
  return true;
}

template <typename N, typename E>
bool gdwg::Graph<N, E>::Replace(const N& oldData, const N& newData){
  if(!IsNode(oldData)){
    throw std::runtime_error("Cannot call Graph::Replace on a node that doesn't exist");
  }

  if(IsNode(newData)){
    return false;
  }

  N* old = getNode(oldData);
  *old = newData;
  return true;
}


