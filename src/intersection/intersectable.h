
#pragma once

#include <spatial_acceleration/boundable.h>
#include "ray.h"
#include "intersection.h"

template <typename T>
class IntersectableInterface {
  
  virtual Intersection<T> GetIntersection(const Ray<T>& ray) const = 0;

};

template <typename G, typename T = typename G::val_t>
class IntersectableBase : 
  public Boundable<G>, 
  public IntersectableInterface<T> {

};