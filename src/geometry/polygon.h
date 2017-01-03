
#pragma once

#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "primitive.h"

class Polygon : public Primitive {
  
  public:

  Polygon() {}
  Polygon(unsigned int count) {
    positions.reserve(count);
    normals.reserve(count);
    uvs.reserve(count);
  }

  std::vector<glm::vec3> positions; // counter-clockwise winding order
  std::vector<glm::vec3> normals;
  std::vector<glm::vec2> uvs;
};