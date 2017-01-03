
#pragma once

#include <spatial_acceleration/bvh_tree.h>

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

template <unsigned int B, unsigned int L>
Intersection Traverse(const accel::BVH<accel::Triangle, B, L> &tree, int i, const Ray& ray) {
  // TODO: vectorize this
  // std::cout << i << std::endl;
  typedef typename accel::TreeBase<accel::Triangle, B, L, accel::BVH<accel::Triangle, B, L>>::node_t node_t;
  const node_t& n = i < 0 ? tree.root : tree.nodes[i];

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

}