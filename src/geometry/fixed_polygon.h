
#pragma once

#include <glm/vec3.hpp>
#include "primitive.h"

template <unsigned int C>
class FixedPolygon : public Primitive {

  public:

  FixedPolygon() {}

  glm::vec3 points[C];  // counter-clockwise winding order
  glm::vec3 normals[C];
};

using Triangle = FixedPolygon<3>;
using Quad = FixedPolygon<4>;