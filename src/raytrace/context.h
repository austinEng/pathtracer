
#pragma once

#include <spatial_acceleration/bvh_tree.h>
#include <spatial_acceleration/triangle.h>
#include <intersection/ray.h>
#include <intersection/intersection.h>
#include "traversal.h"

namespace rt {

class Context {

  public:
  
  accel::BVH<
    accel::Triangle, // primitive type
    accel::BranchingFactor<4, 4>, // branching factor
    accel::BranchingFactor<4, 1> // storage
  > bvhTree;

  void initialize(std::vector<Polygon> &polygons) {
    bvhTree.build(polygons);
  }

  std::vector<Intersection> trace(std::vector<Ray> &rays) {
    std::vector<Intersection> intersections(rays.size());

    for (unsigned int i = 0; i < rays.size(); ++i) {
      Traverse(bvhTree, rays[i], intersections[i]);
    }

    return intersections;
  }
  
  template <unsigned int N>
  std::vector<Intersection> trace(std::vector<RayPacket<N>> &ray_packets) {
    std::vector<Intersection> intersections(N * ray_packets.size());

    for (unsigned int i = 0; i < ray_packets.size(); ++i) {
      Traverse(bvhTree, ray_packets[i], intersections.data() + N);
    }

    return intersections;
  }

};

}
