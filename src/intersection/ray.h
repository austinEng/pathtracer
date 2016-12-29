
#pragma once

#include <glm/vec3.hpp>

struct Ray {
  glm::vec3 pos;
  glm::vec3 dir;

  // template<typename... Ts>
  // Ray(Ts... args) {
  //   static T init[2*3] = {args...};
  //   memcpy(pos.vals, init, 3*sizeof(T));
  //   memcpy(dir.vals, init + 3, 3*sizeof(T));
  // }
};