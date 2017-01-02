
#pragma once

#include <spatial_acceleration/boundable.h>
#include "ray.h"
#include "intersection.h"

class IntersectableInterface : public virtual BoundableInterface {
  public:
  
  virtual Intersection GetIntersection(const Ray& ray) const = 0;

};

template <typename G>
class IntersectableBase :  
  public Boundable<G>,
  public virtual IntersectableInterface {

};