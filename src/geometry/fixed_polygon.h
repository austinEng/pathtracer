
#pragma once

#include <core/vector.h>
#include "primitive.h"

template <unsigned int C, unsigned int D, typename T>
class FixedPolygon : public Primitive<D, T> {

  public:
  typedef T val_t;
  static const unsigned int dim = D;

  typedef Vector<D, T> point_t;
  FixedPolygon() {}

  point_t points[C];
  point_t normals[C];
};

template <unsigned int D, typename T>
using Triangle = FixedPolygon<3, D, T>;

template <unsigned int D, typename T>
using Quad = FixedPolygon<4, D, T>;