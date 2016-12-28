
#pragma once

#include "vector.h"
#include <cstdarg>

template <unsigned int D, typename T>
T Dot(const Vector<D, T> &v1, const Vector<D, T> &v2) {
  T vals[D];
  for (unsigned int d = 0; d < D; ++d) {
    vals[d] = v1[d] * v2[d];
  }
  T out = 0;
  for (unsigned int d = 0; d < D; ++d) {
    out += vals[d];
  }
  return out;
}

template <typename T>
Vector<3, T> Cross(const Vector<3, T> &v1, const Vector<3, T> &v2) { 
  Vector<3, T> out(
    v1[1]*v2[2] - v1[2]*v2[1],
    v1[2]*v2[0] - v1[0]*v2[2],
    v1[0]*v2[1] - v1[1]*v2[0]
  );

  return out;
}
