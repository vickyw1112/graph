
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
