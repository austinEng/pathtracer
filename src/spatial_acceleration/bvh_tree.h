
#pragma once

#include "spatial_tree.h"

template <int D, typename T>
class BVHTree : public SpatialTree<D, T> {
  public:
  
  typedef Bound<D, T> bound_t;
  typedef BoundableInterface<D, T> object_t;
  typedef typename SpatialTree<D, T>::Node node_t;

  enum Mode {
    SAH, // http://www.sci.utah.edu/~wald/Publications/2007/ParallelBVHBuild/fastbuild.pdf
    LBVH // https://devblogs.nvidia.com/parallelforall/thinking-parallel-part-iii-tree-construction-gpu/
  };

  struct Config {
    Mode mode = SAH;
    unsigned int binCount = 12;
  };

  struct Primitive {
    bound_t bound;
    Vector<D, T> centroid;
    std::shared_ptr<object_t> object;

    Primitive(std::shared_ptr<object_t> &obj) {
      object = obj;
      bound = obj->GetBound();
      centroid = obj->GetCentroid();
    }
  };

  BVHTree();

  BVHTree(Config &config);

  virtual node_t* internalBuild(std::vector<std::shared_ptr<object_t>> &objects);

  private:
  Config config;
  int nodeCount;

  node_t* recbuild(std::vector<Primitive> &prims, unsigned int begin, unsigned int end);
  void sortPrimitives(std::vector<Primitive> &prims);
  // void findSplit(std::vector<Primitive> &objects, unsigned int begin, unsigned int end, unsigned int &axis, T &split);
};

#include "bvh_tree.inl"