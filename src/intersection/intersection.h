
#pragma once

template <unsigned int D, typename T>
struct Intersection {
  Vector<D, T> point;
  Vector<D, T> normal;
  Vector<D, T> tangent;
  Vector<D, T> binormal;
  T t;
};