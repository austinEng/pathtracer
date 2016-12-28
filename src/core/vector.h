
#pragma once

#include <cstring>
#include <iostream>

template <unsigned int D, typename T>
struct Vector {
  T vals[D];

  Vector() {
    std::memset(vals, 0, D * sizeof(T));
  }
  
  template<typename... Ts>
  Vector(Ts... args) {
    T init[D] = {args...};
    memcpy(vals, init, D*sizeof(T));
  }

  const T& operator[](unsigned int i) const {
    return vals[i];
  }

  T& operator[](unsigned int i) {
    return vals[i];
  }
};

template <typename T>
using Vector1 = Vector<1, T>;

template <typename T>
using Vector2 = Vector<2, T>;

template <typename T>
using Vector3 = Vector<3, T>;

template <typename T>
using Vector4 = Vector<4, T>;

typedef Vector<1, float> Vector1f;
typedef Vector<2, float> Vector2f;
typedef Vector<3, float> Vector3f;
typedef Vector<1, double> Vector1d;
typedef Vector<2, double> Vector2d;
typedef Vector<3, double> Vector3d;

template <unsigned int D, typename T>
std::ostream& operator<<(std::ostream& os, const Vector<D, T>& v) {    
  os << "Vec" << D << ":";
  for (unsigned int i = 0; i < D; ++i) {
    os << " " << v[i];
  }
  return os;  
}  