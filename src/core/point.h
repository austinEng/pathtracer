
#pragma once

template <unsigned int D, typename T>
struct Point {
  T vals[D];

  Point() {
    std::memset(vals, 0, D * sizeof(T));
  }
};

typedef Point<1, float> Point1f;
typedef Point<2, float> Point2f;
typedef Point<3, float> Point3f;
typedef Point<1, double> Point1d;
typedef Point<2, double> Point2d;
typedef Point<3, double> Point3d;