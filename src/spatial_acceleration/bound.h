
#pragma once

#include <iostream>
#include <limits>

class Bound {
private:
  float dims[6];

public:

  Bound() {
    for (unsigned int d = 0; d < 3; ++d) {
      min(d) = std::numeric_limits<float>::infinity();
    }
    for (unsigned int d = 0; d < 3; ++d) {
      max(d) = -std::numeric_limits<float>::infinity();
    }
  }

  float& min(unsigned int i) {
    return dims[i];
  }

  float& max(unsigned int i) {
    return dims[i + 3];
  }

  float min(unsigned int i) const {
    return dims[i];
  }

  float max(unsigned int i) const {
    return dims[i + 3];
  }

  float extent(unsigned int i) const {
    float e = max(i) - min(i);
    if (e < 0) return 0;
    return e;
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
    for (unsigned int d = 0; d < 3; ++d) {
      if (other.min(d) < min(d)) min(d) = other.min(d);
      if (other.max(d) > max(d)) max(d) = other.max(d);
    }
  }

  void merge(const glm::vec3& point) {
    for (unsigned int d = 0; d < 3; ++d) {
      if (point[d] < min(d)) min(d) = point[d];
      if (point[d] > max(d)) max(d) = point[d];
    }
  }

  static Bound Union(const Bound& b1, const Bound& b2) {
    Bound b = b1;
    b.merge(b2);
    return b;
  }
};

std::ostream& operator<<(std::ostream& os, const Bound& b) {    
  os << "Bound" << std::endl;
  for (unsigned int i = 0; i < 3; ++i) {
    os << "  " << i << ": " << b.min(i) << " --> " << b.max(i) << std::endl;
  }
  return os;  
}  