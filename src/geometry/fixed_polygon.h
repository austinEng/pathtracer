
#pragma once

#include "point.h"


template <unsigned int C, unsigned int D, typename T>
class FixedPolygon {

  public:
  typedef T val_t;
  static const unsigned int dim = D;

  typedef Point<D, T> point_t;
  FixedPolygon() {}

  point_t points[C];
  
};