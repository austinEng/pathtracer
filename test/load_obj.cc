#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION

#include <geometry/polygon.h>
#include <importers/obj_loader.h>

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cout << "Usage: obj_file" << std::endl;
    return 1;
  }
  
  typedef Polygon polygon_t;

  std::vector<std::shared_ptr<Polygon>> polygons;

  std::cout << "Loading obj file " << argv[1] << std::endl;
  polygons = LoadObj<Polygon, Polygon>(argv[1]);

  return 0;
}