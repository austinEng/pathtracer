
#pragma once

#include <iostream>
#include <limits>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <ae_core/simd/types.h>

namespace accel {

class Bound {
#ifdef PAD_BOUNDS
#define N 4
#else
#define N 3
#endif
private:
  union {
    float dims[2*N];
    struct {
      SIMD::Float<N> _min;
      SIMD::Float<N> _max;
    };
  };
  

public:

  Bound() {
    for (unsigned int d = 0; d < N; ++d) {
      min(d) = std::numeric_limits<float>::max();
    }
    for (unsigned int d = 0; d < N; ++d) {
      max(d) = std::numeric_limits<float>::lowest();
    }
  }

  float& min(unsigned int i) {
    return _min[i];
  }

  float& max(unsigned int i) {
    return _max[i];
  }

  float min(unsigned int i) const {
    return _min[i];
  }

  float max(unsigned int i) const {
    return _max[i];
  }

  float extent(unsigned int i) const {
    return max(i) - min(i);
  }

  unsigned int greatestExtent() const {
    unsigned int i = 0;
    for (unsigned int d = 0; d < 3; ++d) {
      if (extent(d) > extent(i)) {
        i = d;
      }  
    }
    return i;
  }

  void merge(const Bound& other) {
    SIMD::Float<N> lmask = other._min < _min;
    SIMD::Float<N> umask = other._max > _max;

    for (unsigned int d = 0; d < N; ++d) {
      min(d) = other.min(d) * lmask[d] + min(d) * !lmask[d];
      max(d) = other.max(d) * umask[d] + max(d) * !umask[d];
    }
  }

  void merge(const glm::vec3& point) {

    #ifdef PAD_BOUNDS
    glm::vec4 p(point, 1);
    SIMD::Float<N>* pt = reinterpret_cast<SIMD::Float<N>*>(&p);
    #else
    const SIMD::Float<N>* pt = reinterpret_cast<const SIMD::Float<N>*>(&point);
    #endif

    SIMD::Float<N> lmask = *pt < _min;
    SIMD::Float<N> umask = *pt > _max;

    for (unsigned int d = 0; d < N; ++d) {
      min(d) = (*pt)[d] * lmask[d] + min(d) * !lmask[d];
      max(d) = (*pt)[d] * umask[d] + max(d) * !umask[d];
    }
  }

  static Bound Union(const Bound& b1, const Bound& b2) {
    Bound b = b1;
    b.merge(b2);
    return b;
  }

  #undef N
};

std::ostream& operator<<(std::ostream& os, const Bound& b) {    
  os << "Bound" << std::endl;
  for (unsigned int i = 0; i < 3; ++i) {
    os << "  " << i << ": " << b.min(i) << " --> " << b.max(i) << std::endl;
  }
  return os;  
}  

template <unsigned int N>
class BoundGroup {

  union {
    float dims[6*N];
    struct {
      SIMD::Float<N> _minX;
      SIMD::Float<N> _minY;
      SIMD::Float<N> _minZ;
      SIMD::Float<N> _maxX;
      SIMD::Float<N> _maxY;
      SIMD::Float<N> _maxZ;
    };
  };

  public:

  BoundGroup() {
    for (unsigned int i = 0; i < 3*N; ++i) {
      dims[i] = std::numeric_limits<float>::max();
    }
    for (unsigned int i = 3*N; i < 6*N; ++i) {
      dims[i] = std::numeric_limits<float>::lowest();
    }
  }

  float& min(unsigned int n, unsigned int i) {
    return dims[i*N + n];
  }

  float& max(unsigned int n, unsigned int i) {
    return dims[i*N + n + 3*N];
  }

  float min(unsigned int n, unsigned int i) const {
    return dims[i*N + n];
  }

  float max(unsigned int n, unsigned int i) const {
    return dims[i*N + n + 3*N];
  }

  float& minX(unsigned int n) {
    return _minX[n];
  }

  float minX(unsigned int n) const {
    return _minX[n];
  }

  float& minY(unsigned int n) {
    return _minY[n];
  }

  float minY(unsigned int n) const {
    return _minY[n];
  }

  float& minZ(unsigned int n) {
    return _minZ[n];
  }

  float minZ(unsigned int n) const {
    return _minZ[n];
  }

  float& maxX(unsigned int n) {
    return _maxX[n];
  }

  float maxX(unsigned int n) const {
    return _maxX[n];
  }

  float& maxY(unsigned int n) {
    return _maxY[n];
  }

  float maxY(unsigned int n) const {
    return _maxY[n];
  }

  float& maxZ(unsigned int n) {
    return _maxZ[n];
  }

  float maxZ(unsigned int n) const {
    return _maxZ[n];
  }
};

template <unsigned int N>
std::ostream& operator<<(std::ostream& os, const BoundGroup<N>& b) {    
  os << "Bound" << N << std::endl;
  for (unsigned int i = 0; i < N; ++i) {
    os << "  " << i << ":" << std::endl;
    os << "    " <<  b.minX(i) << " --> " << b.maxX(i) << std::endl;
    os << "    " <<  b.minY(i) << " --> " << b.maxY(i) << std::endl;
    os << "    " <<  b.minZ(i) << " --> " << b.maxZ(i) << std::endl;
  }
  return os;  
}  

}