
#pragma once

#include <bitset>
#include "config.h"

namespace accel {

template <typename Branch>
class alignas(64) BuildNode {
public:
  Bound bound;
  union {
    BuildNode* children[Branch::NODE];
    int primitives[Branch::LEAF];
  };
  bool isLeaf;
};
// template <typename Branch, typename Node>
// class alignas(64) NodeBase {
// public:
//   static const int count = Branch::NODE;

//   NodeBase();
//   NodeBase(BuildNode<Branch>* nodes[Branch::NODE]) : ([nodes] {

//   }())

// private:
//   NodeBase& FromBuildNodes

  // BoundGroup<Branch::NODE> bound;
  // union {
  //   SIMD::Int<Branch::NODE> children;
  //   SIMD::Int<Branch::NODE> primitives;
  // };
  // std::bitset<Branch::NODE> isLeaf;
// }

// template <typename Branch>
// class alignas(64) Node {
// public:
//   Bound bound;
//   union {
//     int children[Branch::NODE];
//     int primitives[Branch::LEAF];
//   };
//   bool isLeaf;

//   Node();
//   Node(const BuildNode<Branch> &node);
// };

// count = (Branch + N - 1) / N


template <unsigned int N>
class NodeGroup {
public:
  static const int count = N;

  template <typename Branch>
  class alignas(64) NodeGroupImpl {
    public:
    
    BoundGroup<N> bound;
    // union {
    //   SIMD::Int<branch_node> children_groups[N];    
    //   SIMD::Int<branch_node*N> children; // indices of child nodes
    //   SIMD::Int<branch_leaf> primitive_groups[N];
    //   SIMD::Int<branch_leaf*N> primitives;
    // };
    struct {
      union {
        SIMD::Int<Branch::NODE> children;
        SIMD::Int<Branch::LEAF> primitives;
      };
    } groups[N];
    

    std::bitset<N> isLeaf;

    NodeGroupImpl();

    template <unsigned int L>
    NodeGroupImpl(BuildNode<accel::BranchingFactor<N, L>>* nodes[N]) {
      for (unsigned int i = 0; i < N; ++i) {
        if (nodes[i] != nullptr) {
          bound.minX(i) = nodes[i]->bound.min(0);
          bound.minY(i) = nodes[i]->bound.min(1);
          bound.minZ(i) = nodes[i]->bound.min(2);
          bound.maxX(i) = nodes[i]->bound.max(0);
          bound.maxY(i) = nodes[i]->bound.max(1);
          bound.maxZ(i) = nodes[i]->bound.max(2);
        }
      }

      for (unsigned int i = 0; i < N; ++i) {
        isLeaf.set(i, nodes[i] != nullptr && nodes[i]->isLeaf);
      }

      for (unsigned int i = 0; i < N; ++i) {
        groups[i].children = -1;
      }
    }
  };
  
  template <typename Branch>
  using Impl = NodeGroupImpl<Branch>;
};

}