#include <algorithm>
#include "morton.h"

template <int D, typename T>
BVHTree<D, T>::BVHTree() { }

template <int D, typename T>
BVHTree<D, T>::BVHTree(Config &config) {
  this->config = config;  
}


template <int D, typename T>
typename BVHTree<D, T>::node_t* BVHTree<D, T>::recbuild(std::vector<Primitive> &prims, unsigned int begin, unsigned int end) {
  // std::cout << begin << " - " << end << std::endl;
  if (end - begin == 0) return nullptr;
  if (end - begin == 1) {
    this->nodeCount++;
    // return new node_t(prims[0].object);
    this->nodes.emplace_back(prims[0].object);
    return &this->nodes.back();
  }

  unsigned int axis = 0;
  T split;
  bool empty = true;

  switch(config.mode) {
    case SAH: {
      
      const float epsilon = 0.99;

      Bound<D, T> centroidBound;
      for (unsigned int i = begin; i < end; ++i) {
        centroidBound.merge(prims[i].centroid);
      }

      axis = centroidBound.greatestExtent();
      empty = centroidBound.extent(axis) == 0;

      if (!empty) {
        Bound<D, T> binBounds[config.binCount];
        for (unsigned int i = begin; i < end; ++i) {
          unsigned int bin = config.binCount * epsilon * (prims[i].centroid.vals[axis] - centroidBound.min(axis)) / centroidBound.extent(axis);
          assert(bin >= 0 && bin < config.binCount);
          binBounds[bin].merge(prims[i].bound);
        }

        T costs[config.binCount - 1];
        for (unsigned int split = 1; split <= config.binCount - 1; ++split) {
          Bound<D, T> lower;
          Bound<D, T> upper;
          for(unsigned int b = 0; b < config.binCount; ++b) {
            if (b < split) {
              lower.merge(binBounds[b]);
            } else if (b > split) {
              upper.merge(binBounds[b]);
            }
          }

          costs[split - 1] = 0;
          for (unsigned int d1 = 0; d1 < D; ++d1) {
            for (unsigned int d2 = d1 + 1; d2 < D; ++d2) {
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

        split = (T)(i + 1) / config.binCount * epsilon * centroidBound.extent(axis) + centroidBound.min(axis);
      }

      break;
    }

    case LBVH: {
      // MORTON(D) morton;
      Morton<3, float, unsigned int> morton;
      break;
    }  
  }

  Primitive* ptr;
  if (!empty) {
    ptr = std::partition(&prims[begin], &prims[end], [&split, &axis](const Primitive &prim) {
      return prim.centroid.vals[axis] < split;
    });
  } else {
    ptr = std::partition(&prims[begin], &prims[end], [&begin, &end, &prims](const Primitive &prim) {
      return (&prim - &prims[begin]) < (&prims[end] - &prims[begin] + 1) / 2;
    });
  }

  unsigned int mid = ptr - prims.data();
  node_t* left = recbuild(prims, begin, mid);
  node_t* right = recbuild(prims, mid, end);

  this->nodeCount++;
  // return new node_t(left, right);
  this->nodes.emplace_back(left, right);
  return &this->nodes.back();
}

template <int D, typename T>
void BVHTree<D, T>::sortPrimitives(std::vector<Primitive> &prims) {
  std::vector<std::pair<unsigned int, Primitive*>> pairs;
  pairs.reserve(prims.size());
}

template <int D, typename T>
typename BVHTree<D, T>::node_t* BVHTree<D, T>::internalBuild(std::vector<std::shared_ptr<object_t>> &objects) {
  this->nodes.clear();
  this->nodes.reserve(objects.size() * 2);
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