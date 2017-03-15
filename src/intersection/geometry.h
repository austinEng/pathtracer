
#pragma once

#include <glm/glm.hpp>
#include <geometry/geometry.h>
#include <spatial_acceleration/geometry.h>
#include "intersectable.h"
#include "intersection.h"
#include <core/util.h>
#include <spatial_acceleration/primitive.h>

Intersection& TriangleIntersection(const Ray &ray, const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2, Intersection &inter) {

  // https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm

  glm::vec3 e0 = p1 - p0;
  glm::vec3 e1 = p2 - p0;
  
  glm::vec3 T = ray.pos - p0;
  glm::vec3 P = glm::cross(ray.dir, e1);
  glm::vec3 Q = glm::cross(T, e0);

  float det = glm::dot(e0, P);
  float denom = 1.f / det;

  float u = glm::dot(T, P) * denom;
  float v = glm::dot(Q, ray.dir) * denom;

  inter.t = glm::dot(Q, e1) * denom;
  inter.hit = (u >=0 && u < 1.f && v >= 0 && u+v < 1.f && !fequal(0.f, det));
  inter.point = ray.pos + inter.t * ray.dir;

  return inter;
}

Intersection& Intersect(const accel::Triangle &prim, const Ray &ray, Intersection &inter) {
  return TriangleIntersection(ray, prim.positions[0], prim.positions[1], prim.positions[2], inter); 
}

template <unsigned int N>
Intersection& Intersect(const accel::TriangleGroup<N> &prims, const Ray &ray, Intersection &inter) {
  // [         p0's         ] [         p1's         ] [         p2's         ]
  // [  xs  ][  ys  ][  zs  ] 
  // [ xxxx ][ yyyy ][ zzzz ]
  SIMD::Float<3*N> scratch1;
  SIMD::Float<3*N> scratch2;
  SIMD::Float<3*N> scratch3;

  SIMD::Float<3*N> &e0 = SIMD::SUBTRACT(prims.positions[1].vals, prims.positions[0].vals, scratch1);
  SIMD::Float<3*N> &e1 = SIMD::SUBTRACT(prims.positions[2].vals, prims.positions[0].vals, scratch2);
  SIMD::Float<3*N> &T = scratch3;

  SIMD::Float<N> &scratch1_1 = *reinterpret_cast<SIMD::Float<N>*>(&scratch1);
  SIMD::Float<N> &scratch1_2 = *reinterpret_cast<SIMD::Float<N>*>((char*)&scratch1 + sizeof(SIMD::Float<N>));
  SIMD::Float<N> &scratch1_3 = *reinterpret_cast<SIMD::Float<N>*>((char*)&scratch1 + sizeof(SIMD::Float<2*N>));

  SIMD::Float<N> &scratch3_1 = *reinterpret_cast<SIMD::Float<N>*>(&scratch3);
  SIMD::Float<N> &scratch3_2 = *reinterpret_cast<SIMD::Float<N>*>((char*)&scratch3 + sizeof(SIMD::Float<N>));
  SIMD::Float<N> &scratch3_3 = *reinterpret_cast<SIMD::Float<N>*>((char*)&scratch3 + sizeof(SIMD::Float<2*N>));

  #define X i
  #define Y i+N
  #define Z i+2*N

  for (unsigned int i = 0; i < N; ++i) T[X] = ray.pos.x - prims.positions[0].xs[i];
  for (unsigned int i = 0; i < N; ++i) T[Y] = ray.pos.y - prims.positions[0].ys[i];
  for (unsigned int i = 0; i < N; ++i) T[Z] = ray.pos.z - prims.positions[0].zs[i];

  SIMD::Float<3*N> P; // cross(ray.dir, e2)
  SIMD::Float<3*N> Q; // cross(T, e1)
  for (unsigned int i = 0; i < N; ++i) {
    P[X] = ray.dir.y*e1[Z] - ray.dir.z*e1[Y];
    P[Y] = ray.dir.z*e1[X] - ray.dir.x*e1[Z];
    P[Z] = ray.dir.x*e1[Y] - ray.dir.y*e1[X];

    Q[X] = T[Y]*e0[Z] - T[Z]*e0[Y];
    Q[Y] = T[Z]*e0[X] - T[X]*e0[Z];
    Q[Z] = T[X]*e0[Y] - T[Y]*e0[X];
  }

  SIMD::Float<N> det;

  for (unsigned int i = 0; i < N; ++i) det[i] = e0[X] * P[X] + e0[Y] * P[Y] + e0[Z] * P[Z];
  
  SIMD::Float<N> &denom = scratch1_1;
  SIMD::Float<N> &u = scratch1_2;
  SIMD::Float<N> &v = scratch1_3;
  
  SIMD::DIVIDE(1.f, det, denom);

  for (unsigned int i = 0; i < N; ++i) u[i] = T[X]*P[X] + T[Y]*P[Y] + T[Z]*P[Z];
  SIMD::MULTIPLY(u, denom, u);

  for (unsigned int i = 0; i < N; ++i) v[i] = Q[X]*ray.dir.x + Q[Y]*ray.dir.y + Q[Z]*ray.dir.z;
  SIMD::MULTIPLY(v, denom, v);

  SIMD::Float<N> &t = scratch3_1;
  for (unsigned int i = 0; i < N; ++i) t[i] = Q[X]*e1[X] + Q[Y]*e1[Y] + Q[Z]*e1[Z];
  SIMD::MULTIPLY(t, denom, t);

  #undef X
  #undef Y
  #undef Z

  SIMD::Float<N> &hit = scratch3_2;
  for (unsigned int i = 0; i < N; ++i) hit[i] = (u[i] >= 0 && u[i] < 1.f && v[i] >= 0 && u[i]+v[i] < 1.f && !fequal(0.f, det[i]));

  inter.t = std::numeric_limits<float>::max();
  for (unsigned int i = 0; i < N; ++i) {
    if (hit[i] && t[i] < inter.t && prims.mask[i]) {
      inter.t = t[i];
    }
  }
  inter.hit = any(hit);
  inter.point = ray.pos + inter.t * ray.dir;

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
    for (unsigned int c = 0; c < this->positions.size() - 2; ++c) {
      TriangleIntersection(
        ray, 
        this->positions[c],
        this->positions[c+1],
        this->positions[c+2],
        i2
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
      TriangleIntersection(
        ray, 
        this->positions[c],
        this->positions[c+1],
        this->positions[c+2],
        i2
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
    Intersection inter;
    TriangleIntersection(ray, this->positions[0], this->positions[1], this->positions[2], inter);
    return inter;
  }
  
};