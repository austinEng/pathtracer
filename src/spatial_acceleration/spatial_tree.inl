
template <typename P, unsigned int B, unsigned int L, typename T> template <typename O>
void TreeBase<P, B, L, T>::build(const std::vector<O> &objects) {
  // reserve an area for temporarily storing build nodes
  build_t* arena = (build_t*) malloc(sizeof(build_t) * objects.size() * 2);
  
  // convert objects to vector of primitives
  prims.clear();
  prims.reserve(objects.size());
  for (const O &obj : objects) {
    prims.emplace_back(obj);
  }

  int nodeCount;
  build_t* root = InternalBuild(prims, arena, nodeCount);
  flatten(root, nodeCount);

  std::cout << this->nodes.size() << " nodes" << std::endl;
  free(arena);
  std::cout << this->root.bound << std::endl;
}

template <typename P, unsigned int B, unsigned int L, typename T>
void TreeBase<P, B, L, T>::flatten(build_t* root, unsigned int nodeCount) {

  this->root = *root;

  // flatten nodes into vector
  this->nodes.clear();
  this->nodes.reserve(nodeCount - 1);

  std::deque<build_t*> queue;
  
  std::map<build_t*, node_t*> map;
  map.insert(std::pair<build_t*, node_t*>(root, &this->root));
  
  queue.push_back(root);
  unsigned int idx = 0;
  while(queue.size() > 0) {
    build_t* node = queue.front();
    node_t* current = map.find(node)->second;
    
    queue.pop_front();
    if (!node->isLeaf) {
      for (unsigned int i = 0; i < B; ++i) {
        if (node->children[i] != nullptr) {
          current->children[i] = idx++;
          this->nodes.emplace_back(*node->children[i]);
          queue.push_back(node->children[i]);
          map.insert(std::pair<build_t*, node_t*>(node->children[i], &this->nodes.back()));
        } else {
          idx++;
          current->children[i] = -1;
          this->nodes.emplace_back();
        }
      }
    }
    map.erase(node);

  }

}