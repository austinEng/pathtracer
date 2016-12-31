
#pragma once

#include <glm/vec3.hpp>
#include "primitive.h"

class Polygon : public Primitive {
  
  public:

  Polygon() {}
  Polygon(unsigned int count) {
    positions.resize(count);
    normals.resize(count);
    uvs.resize(count);
  }

  std::vector<glm::vec3> positions; // counter-clockwise winding order
  std::vector<glm::vec3> normals;
  std::vector<glm::vec2> uvs;
};