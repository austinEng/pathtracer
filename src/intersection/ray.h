
#pragma once

#include <core/vector.h>

template <typename T>
struct Ray {
  Vector3<T> pos;
  Vector3<T> dir;

  template<typename... Ts>
  Ray(Ts... args) {
    static T init[2*3] = {args...};
    memcpy(pos.vals, init, 3*sizeof(T));
    memcpy(dir.vals, init + 3, 3*sizeof(T));
  }
};