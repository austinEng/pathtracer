
#pragma once

template <typename T>
struct Intersection {
  Vector3<T> point;
  Vector3<T> normal;
  Vector3<T> tangent;
  Vector3<T> binormal;
  T t;
  bool backface;
  bool hit = false;
};