
#pragma once

#include <stack>
#include <glm/glm.hpp>
#include <ae_core/simd/types.h>
#include <spatial_acceleration/bvh_tree.h>
#include <intersection/bound.h>
/*
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
SIMD::Float<N>& IntersectBoundGroup(const BoundGroup<N> &bound, const Ray &ray, SIMD::Float<N>& out) {
  glm::vec3 dir = 1.0f / ray.dir;

  SIMD::Float<N> mask;
  for (unsigned int i = 0; i < N; ++i) mask[i] = 1.f;
  for (unsigned int i = 0; i < N; ++i) mask[i] *= bound.minX(i) <= ray.pos.x;
  for (unsigned int i = 0; i < N; ++i) mask[i] *= bound.minY(i) <= ray.pos.y;
  for (unsigned int i = 0; i < N; ++i) mask[i] *= bound.minZ(i) <= ray.pos.z;
  for (unsigned int i = 0; i < N; ++i) mask[i] *= bound.maxX(i) > ray.pos.x;
  for (unsigned int i = 0; i < N; ++i) mask[i] *= bound.maxY(i) > ray.pos.y;
  for (unsigned int i = 0; i < N; ++i) mask[i] *= bound.maxZ(i) > ray.pos.z;
  mask = 1.f - mask;

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

  SIMD::min(tx0, tx1, minx);
  SIMD::min(ty0, ty1, miny);
  SIMD::min(tz0, tz1, minz);
  SIMD::max(tx0, tx1, maxx);
  SIMD::max(ty0, ty1, maxy);
  SIMD::max(tz0, tz1, maxz);

  SIMD::Float<N> tn;
  SIMD::Float<N> tf;

  SIMD::max(minz, SIMD::max(minx, miny, tn), tn);
  SIMD::min(maxz, SIMD::min(maxx, maxy, tf), tf);

  for (unsigned int i = 0; i < N; ++i) tn[i] = (tn[i] > tf[i] ? -1.f : tn[i]);

  return SIMD::MULTIPLY(tn, mask, out);
}

template <unsigned int R>
SIMD::Float<R>& IntersectBound(const Bound &bound, const RayPacket<R> &ray_packet, SIMD::Float<R>& out) {
  return out;
}

#if GROUP_ACCELERATION_NODES

template <unsigned int B, unsigned int L>
void InternalTraverse(
      const accel::BVH<accel::Triangle, B, L> &tree, 
      int i, 
      const Ray& ray, 
      Intersection** i1, 
      Intersection** i2) {
        
  typedef typename accel::TreeBase<accel::Triangle, B, L, accel::BVH<accel::Triangle, B, L>>::node_t node_t;
  const node_t& n = tree.nodes[i];

  // check node intersections
  SIMD::Float<B> t;
  IntersectBoundGroup(n.bound, ray, t);
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
      if (ts[i].first >=0 && (ts[i].first < (*i1)->t || !(*i1)->hit)) {
        InternalTraverse(tree, n.children[idx], ray, i1, i2);
        if ((*i2)->hit && ((*i2)->t < (*i1)->t || !(*i1)->hit)) {
          std::swap(*i1, *i2);
        }
      }
    } else {
      // check primitive intersections
      (*i2)->hit = false;
      Intersect(tree.prim_groups[n.primitives[idx]], ray, **i2);
      if ((*i2)->hit && ((*i2)->t < (*i1)->t || !(*i1)->hit)) {
        std::swap(*i1, *i2);
      }
    }
  }
}

#else

template <unsigned int B, unsigned int L>
void InternalTraverse(
      const accel::BVH<accel::Triangle, B, L> &tree, 
      int i, 
      const Ray& ray, 
      Intersection** i1, 
      Intersection** i2) {
        
  typedef typename accel::TreeBase<accel::Triangle, B, L, accel::BVH<accel::Triangle, B, L>>::node_t node_t;
  const node_t& n = tree.nodes[i];

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
      if (ts[i].first >=0 && (ts[i].first < (*i1)->t || !(*i1)->hit)) {
        InternalTraverse(tree, ts[i].second, ray, i1, i2);
        if ((*i2)->hit && ((*i2)->t < (*i1)->t || !(*i1)->hit)) {
          std::swap(*i1, *i2);
        }
      }
    }   
  } else {
    for (unsigned int i = 0; i < L; ++i) {
      if (n.primitives[i] != -1) {
        (*i2)->hit = false;
        Intersect(tree.prims[n.primitives[i]], ray, **i2);
        if ((*i2)->hit && ((*i2)->t < (*i1)->t || !(*i1)->hit)) {
          std::swap(*i1, *i2);
        }
      }
    }
  }

}

#endif

template <unsigned int B, unsigned int L, unsigned int R>
void InternalTraverse(
    const accel::BVH<accel::Triangle, B, L> &tree, 
    int i, 
    const RayPacket<R>& ray_packet, 
    Intersection intersections1[R],
    Intersection intersections2[R],
    unsigned long long &destinations) {

  typedef typename accel::TreeBase<accel::Triangle, B, L, accel::BVH<accel::Triangle, B, L>>::node_t node_t;
  const node_t& n = tree.nodes[i];

  SIMD::Float<B> t;
  IntersectBound(tree.nodes[i].bound, ray_packet, t);

  if (!n.isLeaf) {
    // std::pair<float, int> ts[B];
    // for (unsigned int i = 0; i < B; ++i) {
    //   int idx = n.children[i];
    //   if (idx >= 0) {
    //     SIMD::Float<R> t;
    //     unsigned int j = 0;
    //     while (j < R) {
    //       if (t[j] >= 0.f) break;
    //       j++;
    //     }
    //     if (j == R) {
    //       ts[i] = std::make_pair(-1.f, idx);
    //     } else {
    //       ts[i] = std::make_pair(t[j], idx);
    //     }
    //   } else {
    //     ts[i] = std::make_pair(-1.f, idx);
    //   }
    // }
    // std::sort(std::begin(ts), std::end(ts));

    // for (unsigned int i = 0; i < B; ++i) {
    //   if (ts[i].first >=0 && (ts[i].first < (*i1)->t || !(*i1)->hit)) {
    //     InternalTraverse(tree, ts[i].second, ray, i1, i2);
    //     if ((*i2)->hit && ((*i2)->t < (*i1)->t || !(*i1)->hit)) {
    //       std::swap(*i1, *i2);
    //     }
    //   }
    // }

  } else {
    // Ray ray;
    for (unsigned int i = 0; i < L; ++i) {
      if (n.primitives[i] != -1) {
        for (unsigned int j = 0; j < R; ++j) {
          // if (ray_packet.mask[j]) {
          //   ray.pos.x = ray_packet.pos.x[j];
          //   ray.pos.y = ray_packet.pos.y[j];
          //   ray.pos.z = ray_packet.pos.z[j];
          //   ray.dir.x = ray_packet.dir.x[j];
          //   ray.dir.y = ray_packet.dir.y[j];
          //   ray.dir.z = ray_packet.dir.z[j];

          //   Intersection& i1 = destinations & (1 << j) ? intersections1[j] : intersections2[j];
          //   Intersection& i2 = destinations & (1 << j) ? intersections2[j] : intersections1[j];
          //   i2.hit = false;
          //   Intersect(tree.prims[n.primitives[i]], ray, i2);
          //   if (i2.hit && (i2.t < i1.t || !i1.hit)) {
          //     destinations ^= (1 << j);
          //   }
          // }
        }
      }
    }
  }
}

template <unsigned int B, unsigned int L>
void Traverse(
      const accel::BVH<accel::Triangle, B, L> &tree, 
      int i, 
      const Ray& ray, 
      Intersection &inter) {

  typedef typename accel::TreeBase<accel::Triangle, B, L, accel::BVH<accel::Triangle, B, L>>::node_t node_t;
  const node_t& n = tree.nodes[i];

  Intersection intersections[2];
  Intersection* i1 = &intersections[0];
  Intersection* i2 = &intersections[1];

  InternalTraverse(tree, i, ray, &i1, &i2);
  inter = *i1;
}

template <unsigned int B, unsigned int L, unsigned int R>
void Traverse(const accel::BVH<accel::Triangle, B, L> &tree, int i, const RayPacket<R>& ray_packet, Intersection intersections[R]) {
  typedef typename accel::TreeBase<accel::Triangle, B, L, accel::BVH<accel::Triangle, B, L>>::node_t node_t;
  const node_t& n = tree.nodes[i];

  assert(R <= sizeof(unsigned long long));
  Intersection intersections2[R];
  unsigned long long destinations = ~0;

  InternalTraverse(tree, i, ray_packet, intersections, intersections2, destinations);
}

}
*/

namespace rt {

using namespace accel;

template <typename Primitive, typename Branch, unsigned int N, typename PrimitiveStorage>
void Traverse(const BVH<Primitive, Branch, NodeStorage<NodeGroup<N>, PrimitiveStorage>> &tree, const Ray& ray, Intersection &inter) {

  // typedef typename accel::TreeBase<Primitive, Branch, Storage, accel::BVH<Primitive, Branch, Storage>>::node_storage_t node_t;
  // Intersection intersections[2];
  // Intersection* i1 = &intersections[0];
  // Intersection* i2 = &intersections[1];

  // InternalTraverse(tree, 0, ray, &i1, &i2);
  // inter = *i1;
  std::stack<unsigned int> stack;
  stack.push(0);
  while (!stack.empty()) {
    const auto* node = &tree.nodes[stack.top()];
    stack.pop();

    if (node->isLeaf.any()) {
      for (unsigned int i = 0; i < N; ++i) {
        if (node->isLeaf[i]) {
          // Intersect(tree.prims[node.primitives[i]], ray, inter);
        }
      }
    }

    if (!node->isLeaf.all()) {
      std::bitset<N> mask = Hit(node->bound, ray);

      unsigned int hits = 0;
      unsigned int first;
      for (unsigned int i = 0; i < N; ++i) {
        if (mask[i] && (hits++) == 0) first = i; 
      }

      if (hits == 1) {
        node = &tree.nodes[node->children[first]];
        continue;
      } else if (hits > 0) {
        // sort by distance
        std::pair<float, unsigned int> ts[hits];
        SIMD::Float<N> t = DistanceTo(node->bound, ray);

        unsigned int j = 0;
        for (unsigned int i = 0; i < N; ++i) {
          if (mask[i]) ts[j++] = std::make_pair(t[i], i);
        }
        std::sort(ts, ts + hits);

        for (unsigned int i = 0; i < hits; ++i) {
          stack.push(node->children[ts[i].second]);
        }
      }
    }
  }
}

}