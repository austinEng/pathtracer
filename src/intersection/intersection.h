
#pragma once

#include <glm/vec3.hpp>

struct Intersection {
  glm::vec3 point;
  glm::vec3 normal;
  // Vector3<T> tangent;
  // Vector3<T> binormal;
  float t;
  // bool backface;
  bool hit = false;
};