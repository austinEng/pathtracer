#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION

#include <geometry/polygon.h>
#include <spatial_acceleration/geometry.h>
#include <importers/obj_loader.h>
#include <spatial_acceleration/bound.h>
#include <spatial_acceleration/bvh_tree.h>

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cout << "Usage: obj_file" << std::endl;
    return 1;
  }

  std::vector<std::shared_ptr<BoundableInterface>> polygons;

  std::cout << "Loading obj file " << argv[1] << std::endl;
  polygons = LoadObj<BoundableInterface, Boundable<Polygon>>(argv[1]);

  BVHTree<4, 4> tree;
  tree.build(polygons);

  return 0;
}