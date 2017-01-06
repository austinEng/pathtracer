
#pragma once 

#include <glm/vec4.hpp>
#include <ae_core/simd/types.h>
#include <assert.h>
#include "bound.h"

namespace accel {

class Triangle;
template <unsigned int N>
class TriangleGroup {
  public:
  
  union {
    float vals[3*3*N];
    struct {
      union {
        struct {
          SIMD::Float<N> xs;
          SIMD::Float<N> ys;
          SIMD::Float<N> zs;
        };
        SIMD::Float<3*N> vals;
      };
    } positions[3];
  };
  SIMD::Float<N> mask;

  TriangleGroup() {
    for (unsigned int i = 0; i < N; ++i) {
      mask[i] = 0.f;
    }
  }

  void setPrimitive(unsigned int idx, const accel::Triangle &tri);
};

class Triangle {
  public:

  template <unsigned int N>
  struct group {
    typedef TriangleGroup<N> type;
  };

  glm::vec3 positions[3]; // counter-clockwise winding order
  glm::vec3 centroid;
  Bound bound;

  Triangle(const Polygon &poly) {
    unsigned int n = poly.positions.size();
    assert(n == 3);
    for (unsigned int i = 0; i < 3; ++i) {
      positions[i] = poly.positions[i];
      centroid += positions[i] / (float)n;
      bound.merge(positions[i]);
    }
  }
};

template <unsigned int N>
void TriangleGroup<N>::setPrimitive(unsigned int idx, const accel::Triangle &tri) {
  for (unsigned int i = 0; i < 3; ++i) {
    positions[i].xs[idx] = tri.positions[i].x;
    positions[i].ys[idx] = tri.positions[i].y;
    positions[i].zs[idx] = tri.positions[i].z;
  }
  mask[idx] = 1.f;
}

}