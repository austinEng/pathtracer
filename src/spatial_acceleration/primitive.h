
#pragma once 

#include <glm/vec4.hpp>
#include <ae_core/simd/types.h>
#include <assert.h>
#include "bound.h"

namespace accel {

class Triangle {
  public:

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
class TriangleGroup {
  public:
  
  union {
    float vals[3*N];
    struct {
      SIMD::Float<N> xs;
      SIMD::Float<N> ys;
      SIMD::Float<N> zs;
    };
  };
};

}