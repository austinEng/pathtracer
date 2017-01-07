
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
  std::cout << this->root->bound << std::endl;
}

#if GROUP_ACCELERATION_NODES

template <typename P, unsigned int B, unsigned int L, typename T>
void TreeBase<P, B, L, T>::flatten(build_t* root, unsigned int nodeCount) {

  this->nodes.clear();
  this->nodes.reserve((nodeCount - 1) / 4);
  this->prim_groups.reserve(this->prims.size() / L + 1);
  
  typedef struct {
    build_t* nodes[B];
  } build_grp;

  std::deque<build_grp*> queue;

  build_grp* root_group = new build_grp();
  for (unsigned int i = 0; i < B; ++i) {
    root_group->nodes[i] = root->children[i];
  }
  this->nodes.emplace_back(root->children);
  queue.push_back(root_group);
  
  std::map<build_grp*, node_t*> map;
  map.insert(std::pair<build_grp*, node_t*>(root_group, &this->nodes.back()));

  unsigned int idx = 1;
  unsigned int pidx = 0;
  while (queue.size() > 0) {
    build_grp* group = queue.front();
    queue.pop_front();
    node_t* current = map.find(group)->second;
    
    for (unsigned int i = 0; i < B; ++i) {
      if (group->nodes[i] != nullptr) {
        if (!group->nodes[i]->isLeaf) {
          current->children[i] = idx++;
          this->nodes.emplace_back(group->nodes[i]->children);

          build_grp* grp = new build_grp();
          for (unsigned int j = 0; j < B; ++j) {
            grp->nodes[j] = group->nodes[i]->children[j];
          }
          queue.push_back(grp);
          map.insert(std::pair<build_grp*, node_t*>(grp, &this->nodes.back()));
        } else {
          this->prim_groups.emplace_back();
          prim_group_t* grp = &this->prim_groups.back();
          for (unsigned int j = 0; j < L; ++j) {
            int index = group->nodes[i]->primitives[j];
            if (index >= 0) {
              grp->setPrimitive(j, this->prims[index]);
            }
          }
          current->primitives[i] = pidx++;
        }
      }
    }

    map.erase(group);
    delete group;
  }
  // std::cout << this->prims.size() << std::endl;
  this->prims.clear();
  // std::cout << this->prim_groups.size() << std::endl;
  this->root = &this->nodes[0];
}
  
#else

template <typename P, unsigned int B, unsigned int L, typename T>
void TreeBase<P, B, L, T>::flatten(build_t* root, unsigned int nodeCount) {
  
  // flatten nodes into vector
  this->nodes.clear();
  this->nodes.reserve(nodeCount);

  std::deque<build_t*> queue;
  
  this->nodes.emplace_back(*root);

  std::map<build_t*, node_t*> map;
  map.insert(std::pair<build_t*, node_t*>(root, &this->nodes.back()));
  
  queue.push_back(root);
  unsigned int idx = 1;
  while(queue.size() > 0) {
    build_t* node = queue.front();
    queue.pop_front();
    node_t* current = map.find(node)->second;
    
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

  this->root = &this->nodes[0];
}

#endif