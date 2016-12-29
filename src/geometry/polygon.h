
#pragma once

#include <glm/vec3.hpp>
#include "primitive.h"

class Polygon : public Primitive {
  
  public:

  Polygon() {}
  Polygon(unsigned int count) {
    points.resize(count);
    normals.resize(count);
  }

  std::vector<glm::vec3> points; // counter-clockwise winding order
  std::vector<glm::vec3> normals;
};