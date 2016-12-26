
#pragma once

#include "point.h"

template <unsigned int D, typename T>
class Polygon {
  
  public:
  typedef T val_t;
  static const unsigned int dim = D;

  typedef Point<D, T> point_t;
  Polygon(unsigned int count) {
    points.resize(count);
  }

  std::vector<point_t> points;

};