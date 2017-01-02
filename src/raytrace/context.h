
#pragma once

#include <spatial_acceleration/boundable.h>
#include <intersection/ray.h>
#include <intersection/intersection.h>
#include <camera/camera.h>
#include "camera.h"
#include "traversal.h"

namespace rt {

class Context {

  public:

  BVHTree<4, 4> bvhTree;

  void initialize(std::vector<std::shared_ptr<IntersectableInterface>> &polygons) {
    std::vector<std::shared_ptr<BoundableInterface>> boundable(polygons.begin(), polygons.end());
    bvhTree.build(boundable);
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