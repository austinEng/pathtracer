
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

template <unsigned int D, typename T>
T Length(const Vector<D, T> &v) {
  return sqrt(Dot(v, v));
}

template <unsigned int D, typename T>
Vector<D, T> Normalize(const Vector<D, T> &v) {
  Vector<D, T> out;
  T length = Length(v);
  for(unsigned int d = 0; d < D; ++d) {
    out[d] = v[d] / length;
  }
  return out;
}

template <unsigned int D, typename T>
Vector<D, T> operator+(const Vector<D, T> &v1, const Vector<D, T> &v2) {
  Vector<D, T> out;
  for(unsigned int d = 0; d < D; ++d) {
    out[d] = v1[d] + v2[d];
  }
  return out;
}

template <unsigned int D, typename T>
Vector<D, T> operator-(const Vector<D, T> &v1, const Vector<D, T> &v2) {
  Vector<D, T> out;
  for(unsigned int d = 0; d < D; ++d) {
    out[d] = v1[d] - v2[d];
  }
  return out;
}

template <unsigned int D, typename T1, typename T2>
Vector<D, T1> operator*(const Vector<D, T1> &v, T2 scalar) {
  Vector<D, T1> out;
  for(unsigned int d = 0; d < D; ++d) {
    out[d] = v[d] * scalar;
  }
  return out;
}

template <unsigned int D, typename T1, typename T2>
Vector<D, T1> operator*(T2 scalar, const Vector<D, T1> &v) {
  Vector<D, T1> out;
  for(unsigned int d = 0; d < D; ++d) {
    out[d] = v[d] * scalar;
  }
  return out;
}

// Float approximate-equality comparison
template<typename T>
inline bool fequal(T a, T b, T epsilon = 0.0001){
  if (a == b) {
      // Shortcut
      return true;
  }

  const T diff = std::abs(a - b);
  if (a * b == 0) {
      // a or b or both are zero; relative error is not meaningful here
      return diff < (epsilon * epsilon);
  }

  return diff / (std::abs(a) + std::abs(b)) < epsilon;
}
