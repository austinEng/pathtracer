
#pragma once

#include <geometry/point.h>

template <unsigned int D, typename T, typename C>
struct Morton {
  typedef C code_t;
  typedef Point<D, T> point_t;

  static inline unsigned int expandBits(unsigned int v);

  // assumes points are normalized 0 --> 1
  static C mortonCode(const point_t &point) {
    C code = 0;
    const unsigned int componentSize = sizeof(C) / D;
    const T componentMax = std::pow(2, componentSize);
    
    for (unsigned int i = 0; i < D; ++i) {
      T val = std::min(std::max(point.vals[i] * componentMax, 0), componentMax - 1);
      code += expandBits((unsigned int)val) * std::pow(2, i);
    }
    return code;
  } 
};


template <typename T, typename C>
struct Morton<3, T, C> {
  static inline unsigned int expandBits(unsigned int v) {
    v = (v * 0x00010001u) & 0xFF0000FFu;
    v = (v * 0x00000101u) & 0x0F00F00Fu;
    v = (v * 0x00000011u) & 0xC30C30C3u;
    v = (v * 0x00000005u) & 0x49249249u;
    return v;
  }
};