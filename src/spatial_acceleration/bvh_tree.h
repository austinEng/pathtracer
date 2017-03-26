
#pragma once

#include "spatial_tree.h"

namespace accel {

template <typename Primitive, typename Branch, typename Storage>
class BVH : public TreeBase<Primitive, Branch, Storage, BVH<Primitive, Branch, Storage>> {

  class PrimitiveInfo {
  public:
    Bound bound;
    glm::vec3 centroid;
    unsigned int index;

    explicit PrimitiveInfo(const Primitive &primitive) : 
      bound(primitive.getBound()), 
      centroid(primitive.getCentroid()) { }
  };

  public:

  enum Mode {
    SAH, // http://www.sci.utah.edu/~wald/Publications/2007/ParallelBVHBuild/fastbuild.pdf
    LBVH // https://devblogs.nvidia.com/parallelforall/thinking-parallel-part-iii-tree-construction-gpu/
  };

  struct Config {
    Mode mode = SAH;
    unsigned int binCount = 12;
  };

  typedef typename TreeBase<Primitive, Branch, Storage, BVH<Primitive, Branch, Storage>>::build_t node_t;

  node_t* InternalBuild(std::vector<Primitive> &prims, node_t* arena, int &nodeCount);

  private:
  int nodeCount;
  Config config;

  void partition(PrimitiveInfo* prims, unsigned int begin, unsigned int end, unsigned int partitions[2*Branch::NODE]);
  node_t* recbuild(PrimitiveInfo* prims, unsigned int begin, unsigned int end, node_t** arena);

};

#include "bvh_tree.inl"

}