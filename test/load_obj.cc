#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION

#include <geometry/polygon.h>
#include <importers/obj_loader.h>

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cout << "Usage: obj_file" << std::endl;
    return 1;
  }

  std::cout << "Loading obj file " << argv[1] << std::endl;
  std::vector<Polygon> polys = LoadObj<Polygon>(argv[1]);

  return 0;
}