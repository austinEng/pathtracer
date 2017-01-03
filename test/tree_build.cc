#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION

#include <geometry/polygon.h>
#include <spatial_acceleration/geometry.h>
#include <importers/obj_loader.h>
#include <spatial_acceleration/bound.h>
#include <spatial_acceleration/bvh_tree.h>
#include <spatial_acceleration/primitive.h>

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cout << "Usage: obj_file" << std::endl;
    return 1;
  }

  std::cout << "Loading obj file " << argv[1] << std::endl;

  std::vector<Polygon> polys = LoadObj<Polygon>(argv[1]);

  accel::BVH<accel::Triangle, 4, 4> test;
  test.build(polys);

  return 0;
}