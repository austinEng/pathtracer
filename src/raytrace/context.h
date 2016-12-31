
#pragma once

#include <spatial_acceleration/boundable.h>
#include <intersection/ray.h>
#include <intersection/intersection.h>

namespace rt {

class Context {
  
  BVHTree<4, 4> bvhTree;

  public:
  void initialize(std::vector<std::shared_ptr<BoundableInterface>> &polygons) {
    bvhTree.build(polygons);
  }

  std::vector<Intersection> trace(std::vector<Ray> &rays) {
    std::vector<Intersection> intersections;
    intersections.reserve(rays.size());

    return intersections;
  }

};

}