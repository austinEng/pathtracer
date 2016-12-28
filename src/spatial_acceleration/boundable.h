
#pragma once

#include "bound.h"
#include <core/vector.h>

template <unsigned int D, typename T>
class BoundableInterface {
public:

  typedef Bound<D, T> bound_t;
  typedef Vector<D, T> point_t;

  virtual bound_t GetBound() const = 0;
  virtual point_t GetCentroid() const = 0;
};

template <typename G>
class BoundableBase : 
  public G,
  public BoundableInterface<G::dim, typename G::val_t> {
  
};