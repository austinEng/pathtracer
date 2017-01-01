
#pragma once

#include <glm/vec3.hpp>

class Camera {
  public:
  glm::vec3 eye;
  glm::vec3 tgt;
  float fovy;
  unsigned int width;
  unsigned int height;
  float near = 0.001;
};