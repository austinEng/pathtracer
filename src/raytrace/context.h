
#pragma once

#include <spatial_acceleration/bvh_tree.h>
#include <intersection/ray.h>
#include <intersection/intersection.h>
#include "traversal.h"

namespace rt {

class Context {

  public:

  accel::BVH<accel::Triangle, 4, 4> bvhTree;

  void initialize(std::vector<Polygon> &polygons) {
    bvhTree.build(polygons);
  }

  std::vector<Intersection> trace(std::vector<Ray> &rays) {
    std::vector<Intersection> intersections(rays.size());

    for (unsigned int i = 0; i < rays.size(); ++i) {
      intersections[i] = Traverse(bvhTree, -1, rays[i]);
    }

    return intersections;
  }

};

}