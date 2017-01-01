#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
// #include <tiny_obj_loader.h>

#include <geometry/polygon.h>
#include <intersection/geometry.h>
#include <spatial_acceleration/geometry.h>
#include <importers/obj_loader.h>
#include <spatial_acceleration/bound.h>
#include <spatial_acceleration/bvh_tree.h>
#include <raytrace/context.h>

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cout << "Usage: obj_file" << std::endl;
    return 1;
  }

  std::vector<std::shared_ptr<IntersectableInterface>> polygons;

  std::cout << "Loading obj file " << argv[1] << std::endl;
  polygons = LoadObj<IntersectableInterface, Intersectable<Polygon>>(argv[1]);

  rt::Context rtContext;
  rtContext.initialize(polygons);

  rt::Camera<::Camera> camera;
  camera.width = 800;
  camera.height = 600;
  camera.fovy = 45 * 3.14159265358979 / 180;

  Bound& bound = rtContext.bvhTree.root.bound;

  camera.tgt = glm::vec3(
    bound.max(0) - bound.min(0),
    bound.max(1) - bound.min(1),
    bound.max(2) - bound.min(2)
  ) / 2.f;
  camera.eye = camera.tgt + glm::vec3(0,0,1) * bound.extent(1) / 2.f / std::atan(camera.fovy / 2.f);

  std::vector<Ray> rays = std::move(camera.GenerateRays(16));

  std::cout << "Tracing " << rays.size() << " rays" << std::endl;

  std::vector<Intersection> intersections = std::move(rtContext.trace(rays));
  
  return 0;
}