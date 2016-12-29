
#pragma once

#include <glm/glm.hpp>
#include <geometry/geometry.h>
#include <spatial_acceleration/geometry.h>
#include "intersectable.h"
#include "intersection.h"
#include <core/util.h>

Intersection TriangleIntersection(const Ray &ray, const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3) {
  glm::vec3 cross = glm::cross(p2 - p1, p3 - p2);
  float area = glm::length(cross) / 2.f;
  glm::vec3 N = glm::normalize(cross);
  glm::vec3 toSurface = p1 - ray.pos;

  Intersection inter;
  inter.t = glm::dot(N, toSurface) / glm::dot(N, ray.dir);
  inter.normal = N;
  inter.point = ray.pos + inter.t * ray.dir;

  float a1 = glm::length(glm::cross(p1 - inter.point, p2 - inter.point)) / 2.f;
  float a2 = glm::length(glm::cross(p2 - inter.point, p3 - inter.point)) / 2.f;
  float a3 = glm::length(glm::cross(p3 - inter.point, p1 - inter.point)) / 2.f;

  inter.hit = fequal(a1 + a2 + a3, area);

  return inter;
}

template <typename G>
class Intersectable { };

template<>
class Intersectable<Polygon> :
  public IntersectableBase<Polygon> {

  using Polygon::Polygon;

  public:
  
  virtual Intersection GetIntersection(const Ray& ray) const {
    Intersection i1;
    Intersection i2;
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

template <unsigned int C>
class Intersectable<FixedPolygon<C>> :
  public IntersectableBase<FixedPolygon<C>> {

  using FixedPolygon<C>::FixedPolygon;

  public:
  
  virtual Intersection GetIntersection(const Ray& ray) const {
    Intersection i1;
    Intersection i2;
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

template<>
class Intersectable<Triangle> :
  public IntersectableBase<Triangle> {

  using Triangle::Triangle;

  public:
  
  virtual Intersection GetIntersection(const Ray& ray) const {
    return TriangleIntersection(ray, this->points[0], this->points[1], this->points[2]);
  }
  
};