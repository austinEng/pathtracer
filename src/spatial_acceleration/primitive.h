
#pragma once 

#include <assert.h>
#include <bitset>
#include <glm/vec4.hpp>
#include <ae_core/simd/types.h>
#include "bound.h"

namespace accel {

class Triangle {
  public:

  glm::vec3 positions[3]; // counter-clockwise winding order

  Triangle(const Polygon &poly) {
    for (unsigned int i = 0; i < 3; ++i) {
      positions[i] = poly.positions[i];
    }
  }

  Bound getBound() const {
    Bound bound;
    for (unsigned int i = 0; i < 3; ++i) {
      bound.merge(positions[i]);
    }
    return bound;
  }

  glm::vec3 getCentroid() const {
    glm::vec3 centroid;
    for (unsigned int i = 0; i < 3; ++i) {
      centroid += positions[i];
    }
    centroid /= 3;
    return centroid;
  }

};

template <unsigned int N>
class TriangleGroup {
  public:
  
  static const int count = N;

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

  std::bitset<N> mask;

  TriangleGroup(const Triangle* triangles[N]) {
    for (unsigned int i = 0; i < N; ++i) {
      mask.set(i, triangles[i] != nullptr);
      if (triangles[i] != nullptr) { 
        for (unsigned int j = 0; j < 3; ++j) {
          positions[j].xs[i] = triangles[i]->positions[j].x;
          positions[j].ys[i] = triangles[i]->positions[j].y;
          positions[j].zs[i] = triangles[i]->positions[j].z;
        }
      }
    }
  }

};

}