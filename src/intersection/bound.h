
#pragma once

#include <bitset>
#include <ae_core/simd/types.h>
#include <spatial_acceleration/bound.h>
#include "ray.h"

template <unsigned int N>
std::bitset<N> Hit(const accel::BoundGroup<N> &bound, const Ray& ray) {

}

template <unsigned int N>
SIMD::Float<N> DistanceTo(const accel::BoundGroup<N> &bound, const Ray& ray) {
  
}