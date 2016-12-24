
#pragma once

#include "bound.h"
#include <core/point.h>

template <unsigned int D, typename T>
class Boundable {
public:
  typedef Bound<D, T> bound_t;
  typedef Point<D, T> point_t;

  virtual bound_t GetBound() const = 0;
  virtual point_t GetCentroid() const = 0;
};