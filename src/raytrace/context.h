
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
      Traverse(bvhTree, 0, rays[i], intersections[i]);
    }

    return intersections;
  }
  
  template <unsigned int N>
  std::vector<Intersection> trace(std::vector<RayPacket<N>> &ray_packets) {
    std::vector<Intersection> intersections(N * ray_packets.size());

    for (unsigned int i = 0; i < ray_packets.size(); ++i) {
      Traverse(bvhTree, 0, ray_packets[i], intersections.data() + N);
    }

    return intersections;
  }

};

}