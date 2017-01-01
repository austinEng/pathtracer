
#pragma once

#include <glm/vec3.hpp>

struct Ray {
  glm::vec3 pos;
  glm::vec3 dir;

  Ray() { }
  Ray(const glm::vec3 &pos, const glm::vec3 &dir) : pos(pos), dir(dir) { }

};