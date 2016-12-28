
#pragma once

#include <core/vector.h>

template <unsigned int D, typename T>
class SurfaceSampler {

  public:

  enum NormalCalc {
    VERTEX,
    FACE
  };
  
  struct Config {
    bool smooth;
  };

  Config config;

  SurfaceSampler() { }
  SurfaceSampler(Config &config) : config(config) { }
  
  Vector<D, T> SampleNormal() const;
  Vector<D, T> SampleTangent() const;
  Vector<D, T> SampleBinormal() const;
};

template <unsigned int D, typename T> 
class Surface {
  
  public:
  SurfaceSampler<D, T>* surfaceSampler = nullptr;

};