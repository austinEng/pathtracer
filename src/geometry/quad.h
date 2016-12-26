
#pragma once

#include "fixed_polygon.h"

template <unsigned int D, typename T>
class Quad : public FixedPolygon<4, D, T> {
  
  public:
  Quad() { }
};