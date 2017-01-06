
#pragma once

#include <glm/glm.hpp>
#include <geometry/geometry.h>
#include <spatial_acceleration/geometry.h>
#include "intersectable.h"
#include "intersection.h"
#include <core/util.h>
#include <spatial_acceleration/primitive.h>

Intersection TriangleIntersection(const Ray &ray, const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3) {
  // glm::vec3 cross = glm::cross(p2 - p1, p3 - p2);
  // float area = glm::length(cross) / 2.f;
  // glm::vec3 N = glm::normalize(cross);
  // glm::vec3 toSurface = p1 - ray.pos;

  // Intersection inter;
  // inter.t = glm::dot(N, toSurface) / glm::dot(N, ray.dir);
  // inter.normal = N;
  // inter.point = ray.pos + inter.t * ray.dir;

  // float a1 = glm::length(glm::cross(p1 - inter.point, p2 - inter.point)) / 2.f;
  // float a2 = glm::length(glm::cross(p2 - inter.point, p3 - inter.point)) / 2.f;
  // float a3 = glm::length(glm::cross(p3 - inter.point, p1 - inter.point)) / 2.f;

  // inter.hit = fequal(a1 + a2 + a3, area);
  
  // https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
  Intersection inter;

  glm::vec3 e1 = p2 - p1;
  glm::vec3 e2 = p3 - p1;
  
  glm::vec3 T = ray.pos - p1;
  glm::vec3 P = glm::cross(ray.dir, e2);
  glm::vec3 Q = glm::cross(T, e1);

  float det = glm::dot(e1, P);
  float denom = 1.f / det;

  float u = glm::dot(T, P) * denom;
  float v = glm::dot(Q, ray.dir) * denom;

  inter.t = glm::dot(Q, e2) * denom;
  inter.hit = (u >=0 && u < 1.f && v >= 0 && u+v < 1.f && !fequal(0.f, det));
  inter.point = ray.pos + inter.t * ray.dir;

  return inter;
}

Intersection Intersect(const accel::Triangle &prim, const Ray &ray) {
  return TriangleIntersection(ray, prim.positions[0], prim.positions[1], prim.positions[2]); 
}

template <unsigned int N>
Intersection Intersect(const accel::TriangleGroup<N> &prims, const Ray &ray) {
  Intersection i1;
  Intersection i2;

  for (unsigned int i = 0; i < N; ++i) {
    if (prims.mask[i]) {
      i2 = TriangleIntersection(ray,
        glm::vec3(prims.positions[0].xs[i], prims.positions[0].ys[i], prims.positions[0].zs[i]),
        glm::vec3(prims.positions[1].xs[i], prims.positions[1].ys[i], prims.positions[1].zs[i]),
        glm::vec3(prims.positions[2].xs[i], prims.positions[2].ys[i], prims.positions[2].zs[i])
      );
      if (i2.hit && i2.t > 0 && (i2.t < i1.t || !i1.hit)) {
        std::swap(i1, i2);
      }
    }
  }

  return i1;  
  // SIMD::Float<3*N> e1 = prims.positions[1].vals - prims.positions[0].vals;
  // SIMD::Float<3*N> e2 = prims.positions[2].vals - prims.positions[0].vals;
  // SIMD::Float<3*N> T;
  // for (unsigned int i = 0; i < N; ++i) T[i] = ray.pos.x;
  // for (unsigned int i = N; i < 2*N; ++i) T[i] = ray.pos.y;
  // for (unsigned int i = 2*N; i < 3*N; ++i) T[i] = ray.pos.z;
  // T -= prims.positions[1].vals;

  // SIMD::Float<3*N> P;
  // SIMD::Float<3*N> Q;

  // SIMD::Float<N> det;
  // for (unsigned int i = 0; i < N; ++i) det[i] = e1[i] * P[i] + e1[i+N] * P[i+N] + e1[i+2*N] * P[i+2*N];
  // SIMD::Float<N> denom;
  // for (unsigned int i = 0; i < N; ++i) denom[i] = 1.f / det[i];
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
    for (unsigned int c = 0; c < this->positions.size() - 2; ++c) {
      i2 = TriangleIntersection(
        ray, 
        this->positions[c],
        this->positions[c+1],
        this->positions[c+2]
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
        this->positions[c],
        this->positions[c+1],
        this->positions[c+2]
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
    return TriangleIntersection(ray, this->positions[0], this->positions[1], this->positions[2]);
  }
  
};