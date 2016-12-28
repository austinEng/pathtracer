
#pragma once

#include <spatial_acceleration/boundable.h>
#include "ray.h"
#include "intersection.h"

template <unsigned int D, typename T>
class IntersectableInterface {

  virtual Intersection<D, T> GetIntersection(const Ray<D, T>& ray) const = 0;

};

template <typename G>
class IntersectableBase : 
  public Boundable<G>, 
  public IntersectableInterface<G::dim, typename G::val_t> {

};