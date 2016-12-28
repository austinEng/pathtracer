
#pragma once

#include <geometry/geometry.h>
#include <spatial_acceleration/geometry.h>
#include "intersectable.h"
#include "intersection.h"
#include <core/la.h>

template <typename T>
Intersection<T> TriangleIntersection(
    const Ray<T> &ray, const Vector3<T> &p1, const Vector3<T> &p2, const Vector3<T> &p3) {
  Vector3<T> cross = Cross(
    p2 - p1, 
    p3 - p2
  );
  T area = Length(cross) / 2;
  Vector3<T> N = Normalize(cross);
  
  Vector3<T> toSurface = p1 - ray.pos;

  Intersection<T> inter;
  inter.backface = Dot(N, ray.dir) > 0;
  inter.t = Dot(N, toSurface) / Dot(N, ray.dir);
  inter.normal = N;
  inter.point = ray.pos + inter.t * ray.dir;
  
  T a1 = Length(Cross(p1 - inter.point, p2 - inter.point)) / 2;
  T a2 = Length(Cross(p2 - inter.point, p3 - inter.point)) / 2;
  T a3 = Length(Cross(p3 - inter.point, p1 - inter.point)) / 2;
  
  inter.hit = fequal(a1 + a2 + a3, area);

  return inter;
}

template <typename G>
class Intersectable { };


template <typename T>
class Intersectable<Polygon<3, T>> :
  public IntersectableBase<Polygon<3, T>> {

  using Polygon<3, T>::Polygon;

  public:
  
  virtual Intersection<T> GetIntersection(const Ray<T>& ray) const {
    Intersection<T> i1;
    Intersection<T> i2;
    for (unsigned int c = 0; c < this->points.size() - 2; ++c) {
      i2 = TriangleIntersection(
        ray, 
        this->points[c],
        this->points[c+1],
        this->points[c+2]
      );
      if (i2.hit && i2.t > 0 && (i2.t < i1.t || !i1.hit)) {
        std::swap(i1, i2);
      }
    }

    return i1;
  }
  
};

template <unsigned int C, typename T>
class Intersectable<FixedPolygon<C, 3, T>> :
  public IntersectableBase<FixedPolygon<C, 3, T>> {

  using FixedPolygon<C, 3, T>::FixedPolygon;

  public:
  
  virtual Intersection<T> GetIntersection(const Ray<T>& ray) const {
    Intersection<T> i1;
    Intersection<T> i2;
    for (unsigned int c = 0; c < C - 2; ++c) {
      i2 = TriangleIntersection(
        ray, 
        this->points[c],
        this->points[c+1],
        this->points[c+2]
      );
      if (i2.hit && i2.t > 0 && (i2.t < i1.t || !i1.hit)) {
        std::swap(i1, i2);
      }
    }

    return i1;
  }
  
};

template <typename T>
class Intersectable<Triangle<3, T>> :
  public IntersectableBase<Triangle<3, T>> {

  using Triangle<3, T>::Triangle;

  public:
  
  virtual Intersection<T> GetIntersection(const Ray<T>& ray) const {
    return TriangleIntersection(ray, this->points[0], this->points[1], this->points[2]);
  }
  
};