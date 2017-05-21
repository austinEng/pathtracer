
#pragma once

#include <bitset>
#include <deque>
#include "config.h"
#include "bound.h"
#include "node.h"

namespace accel {

template <typename Primitive, typename Branch, typename Storage, typename Tree>
class TreeBase {
public:
  typedef BuildNode<Branch> build_t;
  typedef Primitive prim_t;

  typedef accel::BranchingFactor<
    (Branch::NODE + Storage::NODE - 1) / Storage::NODE,
    (Branch::LEAF + Storage::NODE - 1) / Storage::NODE
  > StorageBranch;

  typedef typename Storage::node_t::template Impl<StorageBranch> node_storage_t;

  typedef typename Storage::prim_t prim_storage_t;

  std::vector<node_storage_t> nodes;
  std::vector<prim_storage_t> prims;

  template <typename Object>
  void build(const std::vector<Object> &objects);

private:
  build_t* InternalBuild(std::vector<prim_t> &prims, build_t* arena, int &nodeCount) {
    return static_cast<Tree*>(this)->InternalBuild(prims, arena, nodeCount);
  }

  void flatten(build_t* root, const std::vector<prim_t> &primitives, unsigned int nodeCount);
};

#include "spatial_tree.inl"

}
