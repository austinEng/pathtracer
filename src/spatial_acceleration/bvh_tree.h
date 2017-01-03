
#pragma once

#include "spatial_tree.h"

namespace accel {

template <typename P, unsigned int B, unsigned int L>
class BVH : public TreeBase<P, B, L, BVH<P, B, L>> {

  public:

  enum Mode {
    SAH, // http://www.sci.utah.edu/~wald/Publications/2007/ParallelBVHBuild/fastbuild.pdf
    LBVH // https://devblogs.nvidia.com/parallelforall/thinking-parallel-part-iii-tree-construction-gpu/
  };

  struct Config {
    Mode mode = SAH;
    unsigned int binCount = 12;
  };

  typedef typename TreeBase<P, B, L, BVH<P, B, L>>::build_t node_t;
  typedef P prim_t;

  node_t* InternalBuild(std::vector<P> &prims, node_t* arena, int &nodeCount);

  private:
  int nodeCount;
  Config config;

  void partition(std::vector<P> &prims, unsigned int begin, unsigned int end, unsigned int partitions[2*B]);
  node_t* recbuild(std::vector<P> &prims, unsigned int begin, unsigned int end, node_t** arena);

};

#include "bvh_tree.inl"

}