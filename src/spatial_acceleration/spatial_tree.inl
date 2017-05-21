
template <typename Primitive, typename Branch, typename Storage, typename Tree> template <typename Object>
void TreeBase<Primitive, Branch, Storage, Tree>::build(const std::vector<Object> &objects) {
  // reserve an area for temporarily storing build nodes
  build_t* arena = (build_t*) malloc(sizeof(build_t) * objects.size() * 2);

  std::vector<prim_t> prims;
  prims.reserve(objects.size());
  for (const Object &obj : objects) {
    prims.emplace_back(obj);
  }

  int nodeCount;
  build_t* root = InternalBuild(prims, arena, nodeCount);
  flatten(root, prims, nodeCount);
  free(arena);
}

template <typename Primitive, typename Branch, typename Storage, typename Tree>
void TreeBase<Primitive, Branch, Storage, Tree>::flatten(build_t* root, const std::vector<prim_t> &primitives, unsigned int nodeCount) {
  this->nodes.clear();
  this->prims.clear();
  this->nodes.reserve((nodeCount - 1) / Storage::node_t::count);
  this->prims.reserve(primitives.size() / Storage::prim_t::count + 1);

  typedef struct build_grp {
    build_t* nodes[Branch::NODE];
    build_grp() { std::fill(nodes, nodes + Branch::NODE, nullptr); }
  } build_grp;

  typedef struct prim_grp {
    const prim_t* prims[Branch::LEAF];
    prim_grp() { std::fill(prims, prims + Branch::LEAF, nullptr); }
  } prim_grp;

  std::deque<build_grp*> queue;

  build_grp* root_group = new build_grp();
  root_group->nodes[0] = root;

  this->nodes.emplace_back(root_group->nodes);
  queue.push_back(root_group);

  std::map<build_grp*, node_storage_t*> map;
  map.insert(std::pair<build_grp*, node_storage_t*>(root_group, &this->nodes.back()));

  unsigned int idx = 1;
  unsigned int pidx = 0;
  while (queue.size() > 0) {
    build_grp* group = queue.front();
    queue.pop_front();
    node_storage_t* current = map.find(group)->second;

    for (unsigned int i = 0; i < Branch::NODE; ++i) {
      if (group->nodes[i] != nullptr) {
        if (!group->nodes[i]->isLeaf) {
          for (unsigned int j = 0; j < Branch::NODE; ++j) {
            // current->groups[i].children[i % Storage::node_t::branch_node] = idx++;
            build_grp* grp = new build_grp();
            for (unsigned int k = 0; k < Storage::node_t::count && j < Branch::NODE; ++j, ++k) {
              grp->nodes[k] = group->nodes[i]->children[j];
            }
            this->nodes.emplace_back(grp->nodes);
            queue.push_back(grp);
            map.insert(std::pair<build_grp*, node_storage_t*>(grp, &this->nodes.back()));
          }
        } else {
          for (unsigned int j = 0; j < Branch::LEAF; ++j) {
            // current->groups[i].primitives[i % Storage::node_t::branch_node] = pidx++;
            prim_grp* grp = new prim_grp();
            for (unsigned int k = 0; k < Storage::node_t::count && j < Branch::NODE; ++j, ++k) {
              grp->prims[k] = &primitives[group->nodes[i]->primitives[j]];
            }
            this->prims.emplace_back(grp->prims);
            delete grp;
          }
        }
      }
    }

    map.erase(group);
    delete group;
  }
}