
#pragma once

template <unsigned int D, typename T>
struct Point {
  T vals[D];

  Point() {
    std::memset(vals, 0, D * sizeof(T));
  }
  
  template<typename... Ts>
  Point(Ts... args) {
    static T init[D] = {args...};
    memcpy(vals, init, D*sizeof(T));
  }

  T operator[](unsigned int i) const {
    return vals[i];
  }

  T& operator[](unsigned int i) {
    return vals[i];
  }
};

typedef Point<1, float> Point1f;
typedef Point<2, float> Point2f;
typedef Point<3, float> Point3f;
typedef Point<1, double> Point1d;
typedef Point<2, double> Point2d;
typedef Point<3, double> Point3d;