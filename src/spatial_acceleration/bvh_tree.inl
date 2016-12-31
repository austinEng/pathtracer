#include <algorithm>
#include "morton.h"

template <unsigned int B, unsigned int L>
BVHTree<B, L>::BVHTree() { }

template <unsigned int B, unsigned int L>
BVHTree<B, L>::BVHTree(Config &config) {
  this->config = config;  
}

template <unsigned int B, unsigned int L>
void BVHTree<B, L>::parition(std::vector<Primitive> &prims, unsigned int begin_, unsigned int end_, unsigned int paritions[2*B]) {
  unsigned int count = 0;
  unsigned int begin = begin_;
  unsigned int end = end_;
  do {
    unsigned int axis = 0;
    float split;
    bool empty = true;
    float costs[2*(config.binCount - 1)];
    unsigned int binSplit = 0;

    switch(config.mode) {
      case SAH: {
        
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

        break;
      }

      case LBVH: {
        // Morton<3, float, unsigned int> morton;
        break;
      }  
    }

    Primitive* ptr;
    if (!empty) {
      ptr = std::partition(&prims[begin], &prims[end], [&split, &axis](const Primitive &prim) {
        return prim.centroid[axis] < split;
      });
    } else {
      ptr = std::partition(&prims[begin], &prims[end], [&begin, &end, &prims](const Primitive &prim) {
        return (&prim - &prims[begin]) < (&prims[end] - &prims[begin] + 1) / 2;
      });
    }

    unsigned int mid = ptr - prims.data();

    if (costs[2*(binSplit)] < costs[2*(binSplit) + 1]) {
      paritions[2*count] = begin;
      paritions[2*count+1] = mid;
      paritions[2*(count+1)] = mid;
      paritions[2*(count+1)+1] = end;

      begin = mid;
      end = end;
    } else {
      paritions[2*count] = mid;
      paritions[2*count+1] = end;
      paritions[2*(count+1)] = begin;
      paritions[2*(count+1)+1] = mid;

      begin = begin;
      end = mid;
    }

    count += 1;

  } while (count < B - 1);
}

template <unsigned int B, unsigned int L>
typename BVHTree<B, L>::node_t* BVHTree<B, L>::recbuild(std::vector<Primitive> &prims, unsigned int begin, unsigned int end) {
  if (end - begin == 0) return nullptr;
  if (end - begin <= L) {
    this->nodeCount++;
    this->nodes.emplace_back();
    node_t* node = &this->nodes.back();
    std::shared_ptr<object_t> objects[L];
    for (unsigned int i = 0; i < L; ++i) {
      if (begin + i < end) {
        objects[i] = prims[begin + i].object;
      } else {
        objects[i] = nullptr;
      }
    }
    node->setObjects(objects);
    return node;
  }

  unsigned int partitions[2*B];
  parition(prims, begin, end, partitions);
  /*
  unsigned int axis = 0;
  float split;
  bool empty = true;

  switch(config.mode) {
    case SAH: {
      
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

        float costs[config.binCount - 1];
        for (unsigned int split = 1; split <= config.binCount - 1; ++split) {
          Bound lower;
          Bound upper;
          for(unsigned int b = 0; b < config.binCount; ++b) {
            if (b < split) {
              lower.merge(binBounds[b]);
            } else if (b > split) {
              upper.merge(binBounds[b]);
            }
          }

          costs[split - 1] = 0;
          for (unsigned int d1 = 0; d1 < 3; ++d1) {
            for (unsigned int d2 = d1 + 1; d2 < 3; ++d2) {
              if (d1 != d2) {
                costs[split - 1] += 2 * lower.extent(d1) * lower.extent(d2);
                costs[split - 1] += 2 * upper.extent(d1) * upper.extent(d2); 
              }
            }
          }
        }

        unsigned int i = 0;
        for (unsigned int c = 0; c < config.binCount - 1; ++c) {
          if (costs[c] < costs[i]) {
            i = c;
          }
        }

        split = (float)(i + 1) / config.binCount * epsilon * centroidBound.extent(axis) + centroidBound.min(axis);
      }

      break;
    }

    case LBVH: {
      // Morton<3, float, unsigned int> morton;
      break;
    }  
  }

  Primitive* ptr;
  if (!empty) {
    ptr = std::partition(&prims[begin], &prims[end], [&split, &axis](const Primitive &prim) {
      return prim.centroid[axis] < split;
    });
  } else {
    ptr = std::partition(&prims[begin], &prims[end], [&begin, &end, &prims](const Primitive &prim) {
      return (&prim - &prims[begin]) < (&prims[end] - &prims[begin] + 1) / 2;
    });
  }

  unsigned int mid = ptr - prims.data();


  node_t* left = recbuild(prims, begin, mid);
  node_t* right = recbuild(prims, mid, end);

  node_t* children[B] = {left, right};
*/
  node_t* children[B];
  for (unsigned int i = 0; i < B; ++i) {
    children[i] = recbuild(prims, partitions[2*i], partitions[2*i+1]);
  }

  this->nodeCount++;
  // node_t* node = new node_t();
  // node->setChildren(children);
  // this->nodes.push_back(node);
  // return node;
  this->nodes.emplace_back();
  node_t* node = &this->nodes.back();
  node->setChildren(children);
  return node;
  // return new node_t(left, right);
  // this->nodes.emplace_back(left, right);
  // return &this->nodes.back();
}

template <unsigned int B, unsigned int L>
void BVHTree<B, L>::sortPrimitives(std::vector<Primitive> &prims) {
  std::vector<std::pair<unsigned int, Primitive*>> pairs;
  pairs.reserve(prims.size());
}

template <unsigned int B, unsigned int L>
typename BVHTree<B, L>::node_t* BVHTree<B, L>::internalBuild(std::vector<std::shared_ptr<object_t>> &objects) {
  this->nodes.clear();
  this->nodes.reserve(objects.size() * 2);
  std::cout << "reserved " << objects.size() * 2 << std::endl;
  this->nodeCount = 0;

  std::vector<Primitive> prims;
  prims.reserve(objects.size());
  for (std::shared_ptr<object_t> &obj : objects) {
    prims.emplace_back(obj);
  }

  switch(config.mode) {
    case LBVH:
      sortPrimitives(prims);
    break;
  }

  node_t* root = recbuild(prims, 0, prims.size());

  std::cout << this->nodes.size() << " nodes" << std::endl;
  return root;
}