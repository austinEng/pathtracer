
#pragma once

#include <iostream>
#include <limits>

class Bound {
private:
  float dims[8];

public:

  Bound() {
    for (unsigned int d = 0; d < 4; ++d) {
      min(d) = std::numeric_limits<float>::max();
    }
    for (unsigned int d = 0; d < 4; ++d) {
      max(d) = std::numeric_limits<float>::lowest();
    }
  }

  float& min(unsigned int i) {
    return dims[i];
  }

  float& max(unsigned int i) {
    return dims[i + 4];
  }

  float min(unsigned int i) const {
    return dims[i];
  }

  float max(unsigned int i) const {
    return dims[i + 4];
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

    // for (unsigned int d = 0; d < 3; ++d) {
    //   if (other.min(d) < min(d)) min(d) = other.min(d);
    //   if (other.max(d) > max(d)) max(d) = other.max(d);
    // }

    int islower[4];
    int isgreater[4];
    for (unsigned int d = 0; d < 4; ++d) {
      islower[d] = other.min(d) < min(d);
      isgreater[d] = other.max(d) > max(d);
    }
    for (unsigned int d = 0; d < 4; ++d) {
      min(d) = other.min(d) * islower[d] + min(d) * !islower[d];
      max(d) = other.min(d) * isgreater[d] + max(d) * !isgreater[d];
    }
  }

  void merge(const glm::vec3& point) {

    // for (unsigned int d = 0; d < 3; ++d) {
    //   if (point[d] < min(d)) min(d) = point[d];
    //   if (point[d] > max(d)) max(d) = point[d];
    // }

    glm::vec4 p(point, 1);
    int islower[4];
    int isgreater[4];
    for (unsigned int d = 0; d < 4; ++d) {
      islower[d] = p[d] < min(d);
      isgreater[d] = p[d] > max(d);
    }
    for (unsigned int d = 0; d < 4; ++d) {
      min(d) = p[d] * islower[d] + min(d) * !islower[d];
      max(d) = p[d] * isgreater[d] + max(d) * !isgreater[d];
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

template <unsigned int N>
class BoundSOA {
  static const int _N = N % 4 == 0 ? N : N + 4 - (N % 4);
  float dims[2*3*_N];
  // x,x,x,_,y,y,y_,z,z,z_,X,X,X,_,Y,Y,Y,_,Z,Z,Z,_

  public:

  float& min(unsigned int n, unsigned int i) {
    return dims[i*_N + n];
  }

  float& max(unsigned int n, unsigned int i) {
    return dims[i*_N + n + 3*_N];
  }

  float min(unsigned int n, unsigned int i) const {
    return dims[i*_N + n];
  }

  float max(unsigned int n, unsigned int i) const {
    return dims[i*_N + n + 3*_N];
  }
};