
#pragma once

#include <spatial_acceleration/bvh_tree.h>
#include <ae_core/simd/types.h>

namespace rt {

float IntersectBound(const Bound &bound, const Ray &ray) {
  // check if we're inside
  if (
    bound.min(0) <= ray.pos.x && ray.pos.x < bound.max(0) &&
    bound.min(1) <= ray.pos.y && ray.pos.y < bound.max(1) &&
    bound.min(2) <= ray.pos.z && ray.pos.z < bound.max(2)
  ) {
    return 0;
  }

  // check intersection
  glm::vec3 dir = 1.0f / ray.dir;
  float tx0 = (bound.min(0) - ray.pos.x) * dir.x;
  float tx1 = (bound.max(0) - ray.pos.x) * dir.x;
  float ty0 = (bound.min(1) - ray.pos.y) * dir.y;
  float ty1 = (bound.max(1) - ray.pos.y) * dir.y;
  float tz0 = (bound.min(2) - ray.pos.z) * dir.z;
  float tz1 = (bound.max(2) - ray.pos.z) * dir.z;

  float minx = glm::min(tx0, tx1);
  float miny = glm::min(ty0, ty1);
  float minz = glm::min(tz0, tz1);
  float maxx = glm::max(tx0, tx1);
  float maxy = glm::max(ty0, ty1);
  float maxz = glm::max(tz0, tz1);

  float tn = glm::max(glm::max(minx, miny), minz);
  float tf = glm::min(glm::min(maxx, maxy), maxz);

  if (tn > tf) { // missed
    return -1;
  }
  return tn;
}

template <unsigned int N>
SIMD::Float<N> IntersectBoundGroup(const BoundGroup<N> &bound, const Ray &ray) {
  glm::vec3 dir = 1.0f / ray.dir;

  SIMD::Float<N> inmask;
  for (unsigned int i = 0; i < N; ++i) inmask[i] = 1.f;
  for (unsigned int i = 0; i < N; ++i) inmask[i] *= bound.minX(i) <= ray.pos.x;
  for (unsigned int i = 0; i < N; ++i) inmask[i] *= bound.minY(i) <= ray.pos.y;
  for (unsigned int i = 0; i < N; ++i) inmask[i] *= bound.minZ(i) <= ray.pos.z;
  for (unsigned int i = 0; i < N; ++i) inmask[i] *= bound.maxX(i) > ray.pos.x;
  for (unsigned int i = 0; i < N; ++i) inmask[i] *= bound.maxY(i) > ray.pos.y;
  for (unsigned int i = 0; i < N; ++i) inmask[i] *= bound.maxZ(i) > ray.pos.z;

  SIMD::Float<N> tx0;
  SIMD::Float<N> tx1;
  SIMD::Float<N> ty0;
  SIMD::Float<N> ty1;
  SIMD::Float<N> tz0;
  SIMD::Float<N> tz1;
  for (unsigned int i = 0; i < N; ++i) tx0[i] = (bound.minX(i) - ray.pos.x) * dir.x;
  for (unsigned int i = 0; i < N; ++i) tx1[i] = (bound.maxX(i) - ray.pos.x) * dir.x;
  for (unsigned int i = 0; i < N; ++i) ty0[i] = (bound.minY(i) - ray.pos.y) * dir.y;
  for (unsigned int i = 0; i < N; ++i) ty1[i] = (bound.maxY(i) - ray.pos.y) * dir.y;
  for (unsigned int i = 0; i < N; ++i) tz0[i] = (bound.minZ(i) - ray.pos.z) * dir.z;
  for (unsigned int i = 0; i < N; ++i) tz1[i] = (bound.maxZ(i) - ray.pos.z) * dir.z;

  SIMD::Float<N> minx;
  SIMD::Float<N> miny;
  SIMD::Float<N> minz;
  SIMD::Float<N> maxx;
  SIMD::Float<N> maxy;
  SIMD::Float<N> maxz;

  for (unsigned int i = 0; i < N; ++i) minx[i] = (tx0[i] < tx1[i]) * tx0[i] + (tx0[i] >= tx1[i]) * tx1[i];
  for (unsigned int i = 0; i < N; ++i) miny[i] = (ty0[i] < ty1[i]) * ty0[i] + (ty0[i] >= ty1[i]) * ty1[i];
  for (unsigned int i = 0; i < N; ++i) minz[i] = (tz0[i] < tz1[i]) * tz0[i] + (tz0[i] >= tz1[i]) * tz1[i];
  for (unsigned int i = 0; i < N; ++i) maxx[i] = (tx0[i] < tx1[i]) * tx1[i] + (tx0[i] >= tx1[i]) * tx0[i];
  for (unsigned int i = 0; i < N; ++i) maxy[i] = (ty0[i] < ty1[i]) * ty1[i] + (ty0[i] >= ty1[i]) * ty0[i];
  for (unsigned int i = 0; i < N; ++i) maxz[i] = (tz0[i] < tz1[i]) * tz1[i] + (tz0[i] >= tz1[i]) * tz0[i];

  SIMD::Float<N> tn;
  SIMD::Float<N> tf;

  for (unsigned int i = 0; i < N; ++i) tn[i] = (minx[i] > miny[i]) * minx[i] + (minx[i] <= miny[i]) * miny[i];
  for (unsigned int i = 0; i < N; ++i) tn[i] = (tn[i] > minz[i]) * tn[i] + (tn[i] <= minz[i]) * minz[i];
  for (unsigned int i = 0; i < N; ++i) tf[i] = (maxx[i] <= maxy[i]) * maxx[i] + (maxx[i] > maxy[i]) * maxy[i];
  for (unsigned int i = 0; i < N; ++i) tf[i] = (tf[i] <= maxz[i]) * tf[i] + (tf[i] > maxz[i]) * maxz[i];

  for (unsigned int i = 0; i < N; ++i) tn[i] = (tn[i] > tf[i]) * -1.f + (tn[i] <= tf[i]) * tn[i];

  for (unsigned int i = 0; i < N; ++i) tn[i] *= inmask[i] != 1.f;

  return tn;

}

#if GROUP_ACCELERATION_NODES

template <unsigned int B, unsigned int L>
Intersection Traverse(const accel::BVH<accel::Triangle, B, L> &tree, int i, const Ray& ray) {
  typedef typename accel::TreeBase<accel::Triangle, B, L, accel::BVH<accel::Triangle, B, L>>::node_t node_t;
  const node_t& n = tree.nodes[i];

  Intersection i1;
  Intersection i2;

  // check node intersections
  SIMD::Float<B> t = IntersectBoundGroup(n.bound, ray);
  std::pair<float, int> ts[B];
  for (unsigned int i = 0; i < B; ++i) {
    int idx = n.children[i];
    if (idx >= 0) {
      ts[i] = std::make_pair(t[i], i);
    } else {
      ts[i] = std::make_pair(-1.f, i);
    }
  }
  std::sort(std::begin(ts), std::end(ts));

  for (unsigned int i = 0; i < B; ++i) {
    unsigned int idx = ts[i].second;
    if (!n.isLeaf[idx]) {
      if (ts[i].first >=0 && (ts[i].first < i1.t || !i1.hit)) {
        i2 = Traverse(tree, n.children[idx], ray);
        if (i2.hit && (i2.t < i1.t || !i1.hit)) {
          std::swap(i1, i2);
        }
      }
    } else {
      // check primitive intersections
      i2 = Intersect(tree.prim_groups[n.primitives[idx]], ray);
      if (i2.hit && (i2.t < i1.t || !i1.hit)) {
        std::swap(i1, i2);
      }
    }
  }

  return i1;

}

#else

template <unsigned int B, unsigned int L>
Intersection Traverse(const accel::BVH<accel::Triangle, B, L> &tree, int i, const Ray& ray) {
  typedef typename accel::TreeBase<accel::Triangle, B, L, accel::BVH<accel::Triangle, B, L>>::node_t node_t;
  const node_t& n = tree.nodes[i];

  Intersection i1;
  Intersection i2;
  if (!n.isLeaf) {

    std::pair<float, int> ts[B];
    for (unsigned int i = 0; i < B; ++i) {
      int idx = n.children[i];
      if (idx >= 0) {
        float t = IntersectBound(tree.nodes[idx].bound, ray);
        ts[i] = std::make_pair(t, idx);
      } else {
        ts[i] = std::make_pair(-1.f, idx);
      }
    }
    std::sort(std::begin(ts), std::end(ts));

    for (unsigned int i = 0; i < B; ++i) {
      if (ts[i].first >=0 && (ts[i].first < i1.t || !i1.hit)) {
        i2 = Traverse(tree, ts[i].second, ray);
        if (i2.hit && (i2.t < i1.t || !i1.hit)) {
          std::swap(i1, i2);
        }
      }
    }   
  } else {
    for (unsigned int i = 0; i < L; ++i) {
      if (n.primitives[i] != -1) {
        i2 = Intersect(tree.prims[n.primitives[i]], ray);
        if (i2.hit && (i2.t < i1.t || !i1.hit)) {
          std::swap(i1, i2);
        }
      }
    }
  }
  
  return i1;
}
#endif

}