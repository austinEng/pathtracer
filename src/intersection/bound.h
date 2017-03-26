
#pragma once

#include <bitset>
#include <ae_core/simd/types.h>
#include <spatial_acceleration/bound.h>
#include "ray.h"

template <unsigned int N>
void DistanceTo(const accel::BoundGroup<N> &bound, const Ray& ray, SIMD::Float<N> &tn, std::bitset<N> &hit_mask) {
  glm::vec3 dir = 1.0f / ray.dir;
  
  SIMD::Float<N> tx0, tx1, ty0, ty1, tz0, tz1;
  for (unsigned int i = 0; i < N; ++i) tx0[i] = (bound.minX(i) - ray.pos.x) * dir.x;
  for (unsigned int i = 0; i < N; ++i) tx1[i] = (bound.maxX(i) - ray.pos.x) * dir.x;
  for (unsigned int i = 0; i < N; ++i) ty0[i] = (bound.minY(i) - ray.pos.y) * dir.y;
  for (unsigned int i = 0; i < N; ++i) ty1[i] = (bound.maxY(i) - ray.pos.y) * dir.y;
  for (unsigned int i = 0; i < N; ++i) tz0[i] = (bound.minZ(i) - ray.pos.z) * dir.z;
  for (unsigned int i = 0; i < N; ++i) tz1[i] = (bound.maxZ(i) - ray.pos.z) * dir.z;

  SIMD::Float<N> minx, miny, minz, maxx, maxy, maxz;

  SIMD::min(tx0, tx1, minx);
  SIMD::min(ty0, ty1, miny);
  SIMD::min(tz0, tz1, minz);
  SIMD::max(tx0, tx1, maxx);
  SIMD::max(ty0, ty1, maxy);
  SIMD::max(tz0, tz1, maxz);

  SIMD::Float<N> tf;

  SIMD::max(minz, SIMD::max(minx, miny, tn), tn);
  SIMD::max(0, tn, tn);
  SIMD::min(maxz, SIMD::min(maxx, maxy, tf), tf);

  for (unsigned int i = 0; i < N; ++i) hit_mask &= (tn[i] <= tf[i]);
}