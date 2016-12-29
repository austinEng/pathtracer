
#pragma once

#include <spatial_acceleration/boundable.h>
#include <intersection/ray.h>
#include <intersection/intersection.h>

namespace rt {

class Context {
  
  BVHTree bvhTree;

  public:
  void initialize(std::vector<std::shared_ptr<BoundableInterface>> &polygons) {
    bvhTree.build(polygons);
  }

  void trace(std::vector<Ray> &rays) {
    
  }

};

}