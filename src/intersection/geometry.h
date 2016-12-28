
#pragma once

#include <geometry/geometry.h>
#include <spatial_acceleration/geometry.h>
#include "intersectable.h"
#include "intersection.h"

template <typename G>
class Intersectable { };


template <typename T>
class Intersectable<Polygon<3, T>> :
  public IntersectableBase<Polygon<3, T>> {

  using Polygon<3, T>::Polygon;

  public:
  
  virtual Intersection<3, T> GetIntersection(const Ray<3, T>& ray) const {
    return Intersection<3, T>();
  }
  
};

template <typename T>
class Intersectable<Triangle<3, T>> :
  public IntersectableBase<Triangle<3, T>> {

  using Triangle<3, T>::Triangle;

  public:
  
  virtual Intersection<3, T> GetIntersection(const Ray<3, T>& ray) const {
    return Intersection<3, T>();
  }
  
};

template <unsigned int C, typename T>
class Intersectable<FixedPolygon<C, 3, T>> :
  public IntersectableBase<FixedPolygon<C, 3, T>> {

  using FixedPolygon<C, 3, T>::Polygon;

  public:
  
  virtual Intersection<3, T> GetIntersection(const Ray<3, T>& ray) const {
    return Intersection<3, T>();
  }
  
};