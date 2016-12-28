
#pragma once

#include <core/vector.h>

template <unsigned int D, typename T>
struct Ray {
  Vector<D, T> pos;
  Vector<D, T> dir;

  template<typename... Ts>
  Ray(Ts... args) {
    static T init[2*D] = {args...};
    memcpy(pos.vals, init, D*sizeof(T));
    memcpy(dir.vals, init + D, D*sizeof(T));
  }
};