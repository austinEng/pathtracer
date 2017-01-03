
#pragma once

#include <deque>
#include "primitive.h"
#include "bound.h"

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
    BoundSoA<B> bounds;
  };

  typedef BuildNode build_t;
  typedef Node node_t;
  typedef P prim_t;

  node_t root;
  std::vector<node_t> nodes;
  std::vector<P> prims;

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
  

