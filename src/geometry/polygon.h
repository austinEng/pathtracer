
#pragma once

#include <core/vector.h>
#include "primitive.h"

template <unsigned int D, typename T>
class Polygon : public Primitive<D, T> {
  
  public:
  typedef T val_t;
  static const unsigned int dim = D;

  typedef Vector<D, T> point_t;
  Polygon() {}
  Polygon(unsigned int count) {
    points.resize(count);
    normals.resize(count);
  }

  std::vector<point_t> points;
  std::vector<point_t> normals;
};