
#pragma once

#include <deque>
#include "primitive.h"
#include "bound.h"
#include <ae_core/simd/types.h>

namespace accel {

template <typename P, unsigned int B, unsigned int L, typename T>
class TreeBase {

public:

  class alignas(64) BuildNode {
    public:
    Bound bound;
    union {
      BuildNode* children[B];
      int primitives[L];
    };
    bool isLeaf;
  };

  class alignas(64) Node {
    public:
    Bound bound;
    union {
      int children[B];
      int primitives[L];
    };
    bool isLeaf;

    Node() {}
    Node(const BuildNode &node) : 
        bound(node.bound), 
        isLeaf(node.isLeaf) {
      for (unsigned int i = 0; i < L; ++i) {
        primitives[i] = node.primitives[i];
      }
    }
  };

  class alignas(64) NodeGroup {
    public:
    NodeGroup() {}
    NodeGroup(BuildNode* nodes[B]) {
      for (unsigned int i = 0; i < B; ++i) {
        if (nodes[i] != nullptr) {
          bound.minX(i) = nodes[i]->bound.min(0);
          bound.minY(i) = nodes[i]->bound.min(1);
          bound.minZ(i) = nodes[i]->bound.min(2);
          bound.maxX(i) = nodes[i]->bound.max(0);
          bound.maxY(i) = nodes[i]->bound.max(1);
          bound.maxZ(i) = nodes[i]->bound.max(2);
        }
      }
      for (unsigned int i = 0; i < B; ++i) {
        if (nodes[i] != nullptr) {
          isLeaf[i] = nodes[i]->isLeaf;
        }
      }
    }
    BoundGroup<B> bound;
    union {
      SIMD::Int<B> children;
      SIMD::Int<B> primitives;
    };
    SIMD::Float<B> isLeaf; // floats for the sake of vector operations
  };

  typedef BuildNode build_t;
  #if GROUP_ACCELERATION_NODES
  typedef NodeGroup node_t;
  #else
  typedef Node node_t;
  #endif
  typedef P prim_t;

  node_t* root;
  std::vector<node_t> nodes;
  std::vector<P> prims;

  #if GROUP_ACCELERATION_NODES
  typedef typename  P::template group<L>::type prim_group_t;
  std::vector<prim_group_t> prim_groups;
  #endif

  template <typename O>
  void build(const std::vector<O> &objects);

private:

  build_t* InternalBuild(std::vector<prim_t> &prims, build_t* arena, int &nodeCount) {
    return static_cast<T*>(this)->InternalBuild(prims, arena, nodeCount);
  }

  void flatten(build_t* root, unsigned int nodeCount);

};

#include "spatial_tree.inl"

}
  

