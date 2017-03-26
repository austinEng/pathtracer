
template <typename Primitive, typename Branch, typename Storage>
typename BVH<Primitive, Branch, Storage>::node_t* BVH<Primitive, Branch, Storage>::InternalBuild(std::vector<Primitive> &prims, node_t* arena, int &nodeCount) {

  std::vector<PrimitiveInfo> primitiveInfo(prims.begin(), prims.end());
  for (unsigned int i = 0; i < primitiveInfo.size(); ++i) {
    primitiveInfo[i].index = i;
  }

  node_t* root = recbuild(primitiveInfo.data(), 0, prims.size(), &arena);
  nodeCount = this->nodeCount;
  
  // Shuffle primitives. Primitive info has been sorted but primitives have not
  for (unsigned int i = 0; i < primitiveInfo.size(); ++i) {
    unsigned int index = primitiveInfo[i].index;
    if (index != i) {
      std::swap(primitiveInfo[i], primitiveInfo[index]);
      std::swap(prims[i], prims[index]);
    }
  }

  return root;
}

template <typename Primitive, typename Branch, typename Storage>
void BVH<Primitive, Branch, Storage>::partition(PrimitiveInfo* prims, unsigned int begin_, unsigned int end_, unsigned int partitions[2*Branch::NODE]) {
  unsigned int count = 0;
  unsigned int begin = begin_;
  unsigned int end = end_;
  do {
    unsigned int axis = 0;
    float split;
    bool empty = true;
    float costs[2*(config.binCount - 1)];
    unsigned int binSplit = 0;
        
    const float epsilon = 0.99;

    Bound centroidBound;
    for (unsigned int i = begin; i < end; ++i) {
      centroidBound.merge(prims[i].centroid);
    }

    axis = centroidBound.greatestExtent();
    empty = centroidBound.extent(axis) == 0;

    if (!empty) {
      Bound binBounds[config.binCount];
      
      for (unsigned int i = begin; i < end; ++i) {
        unsigned int bin = config.binCount * epsilon * (prims[i].centroid[axis] - centroidBound.min(axis)) / centroidBound.extent(axis);
        binBounds[bin].merge(prims[i].bound);
      }

      for (unsigned int split = 0; split < config.binCount - 1; ++split) {
        Bound lower;
        Bound upper;
        for(unsigned int b = 0; b < config.binCount; ++b) {
          if (b <= split) {
            lower.merge(binBounds[b]);
          } else if (b > split) {
            upper.merge(binBounds[b]);
          }
        }

        costs[2*split] = 0;
        costs[2*split + 1] = 0;
        for (unsigned int d1 = 0; d1 < 3; ++d1) {
          for (unsigned int d2 = d1 + 1; d2 < 3; ++d2) {
            if (d1 != d2) {
              costs[2*split] += 2 * lower.extent(d1) * lower.extent(d2);
              costs[2*split + 1] += 2 * upper.extent(d1) * upper.extent(d2); 
            }
          }
        }
      }

      for (unsigned int c = 0; c < config.binCount - 1; ++c) {
        if (costs[2*c] + costs[2*c+1] < costs[2*binSplit] + costs[2*binSplit+1]) {
          binSplit = c;
        }
      }

      split = (float)(binSplit + 1) / config.binCount * epsilon * centroidBound.extent(axis) + centroidBound.min(axis);
    }

    PrimitiveInfo* ptr;
    if (!empty) {
      ptr = std::partition(&prims[begin], &prims[end], [&split, &axis](const PrimitiveInfo &prim) {
        return prim.centroid[axis] < split;
      });
    } else {
      ptr = std::partition(&prims[begin], &prims[end], [&begin, &end, &prims](const PrimitiveInfo &prim) {
        return (&prim - &prims[begin]) < (&prims[end] - &prims[begin] + 1) / 2;
      });
    }

    unsigned int mid = ptr - prims;

    if (costs[2*(binSplit)] < costs[2*(binSplit) + 1]) {
      partitions[2*count] = begin;
      partitions[2*count+1] = mid;
      partitions[2*(count+1)] = mid;
      partitions[2*(count+1)+1] = end;

      begin = mid;
      end = end;
    } else {
      partitions[2*count] = mid;
      partitions[2*count+1] = end;
      partitions[2*(count+1)] = begin;
      partitions[2*(count+1)+1] = mid;

      begin = begin;
      end = mid;
    }

    count += 1;

  } while (count < Branch::NODE - 1);
}

template <typename Primitive, typename Branch, typename Storage>
typename BVH<Primitive, Branch, Storage>::node_t* BVH<Primitive, Branch, Storage>::recbuild(PrimitiveInfo* prims, unsigned int begin, unsigned int end, node_t** arena) {
  if (end - begin == 0) return nullptr;

  this->nodeCount++;

  if (end - begin <= Branch::LEAF) {
    node_t* node = new((*arena)++) node_t();
    node->isLeaf = true;

    for (unsigned int i = 0; i < Branch::LEAF; ++i) {
      if (begin + i < end) {
        node->primitives[i] = begin + i;
        node->bound.merge(prims[begin+i].bound);
      } else {
        node->primitives[i] = -1;
      }
    }
    return node;
  } else {
    unsigned int partitions[2*Branch::NODE];
    partition(prims, begin, end, partitions);

    node_t* node = new((*arena)++) node_t();
    node->isLeaf = false;

    for (unsigned int i = 0; i < Branch::NODE; ++i) {
      node->children[i] = recbuild(prims, partitions[2*i], partitions[2*i+1], arena);
      if (node->children[i] != nullptr) {
        node->bound.merge(node->children[i]->bound);
      }
    }
    return node;
  }
}