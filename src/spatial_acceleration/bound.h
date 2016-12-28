
#pragma once

#include <iostream>
#include <limits>
#include <core/vector.h>

template <unsigned int D, typename T>
class Bound {
private:
  T dims[2*D];

public:

  typedef Vector<D, T> point_t;

  Bound() {
    for (unsigned int d = 0; d < D; ++d) {
      min(d) = std::numeric_limits<T>::infinity();
    }
    for (unsigned int d = 0; d < D; ++d) {
      max(d) = -std::numeric_limits<T>::infinity();
    }
  }

  T& min(unsigned int i) {
    return dims[i];
  }

  T& max(unsigned int i) {
    return dims[i + D];
  }

  T min(unsigned int i) const {
    return dims[i];
  }

  T max(unsigned int i) const {
    return dims[i + D];
  }

  T extent(unsigned int i) const {
    T e = max(i) - min(i);
    if (e < 0) return 0;
    return e;
  }

  unsigned int greatestExtent() const {
    unsigned int i = 0;
    for (unsigned int d = 0; d < D; ++d) {
      if (extent(d) > extent(i)) {
        i = d;
      }  
    }
    return i;
  }

  void merge(const Bound& other) {
    for (unsigned int d = 0; d < D; ++d) {
      if (other.min(d) < min(d)) min(d) = other.min(d);
      if (other.max(d) > max(d)) max(d) = other.max(d);
    }
  }

  void merge(const point_t& point) {
    for (unsigned int d = 0; d < D; ++d) {
      if (point.vals[d] < min(d)) min(d) = point.vals[d];
      if (point.vals[d] > max(d)) max(d) = point.vals[d];
    }
  }

  static Bound Union(const Bound& b1, const Bound& b2) {
    Bound b = b1;
    b.merge(b2);
    return b;
  }
};

template <unsigned int D, typename T>
std::ostream& operator<<(std::ostream& os, const Bound<D, T>& b) {    
  os << "Bound" << D << std::endl;
  for (unsigned int i = 0; i < D; ++i) {
    os << "  " << i << ": " << b.min(i) << " --> " << b.max(i) << std::endl;
  }
  return os;  
}  

typedef Bound<3, float> Bound3f;
typedef Bound<2, float> Bound2f;
typedef Bound<1, float> Bound1f;
typedef Bound<3, double> Bound3d;
typedef Bound<2, double> Bound2d;
typedef Bound<1, double> Bound1d;