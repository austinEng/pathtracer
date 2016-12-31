
#pragma once

#include <geometry/geometry.h>
#include "boundable.h"

template <typename G>
class Boundable { };

template<>
class Boundable<Polygon> : public BoundableBase<Polygon> {

  using Polygon::Polygon;

  public:
  
  virtual Bound GetBound() const {
    Bound bound;

    for (unsigned int d = 0; d < 3; ++d) {
      bound.min(d) = this->positions[0][d];
    }
    for (unsigned int d = 0; d < 3; ++d) {
      bound.max(d) = this->positions[0][d];
    }

    for (unsigned int c = 1; c < this->positions.size(); ++c) {
      for (unsigned int d = 0; d < 3; ++d) {
        if (this->positions[c][d] < bound.min(d)) bound.min(d) = this->positions[c][d];
      }
      for (unsigned int d = 0; d < 3; ++d) {
        if (this->positions[c][d] > bound.max(d)) bound.max(d) = this->positions[c][d];
      }
    }

    return bound;
  }

  virtual glm::vec3 GetCentroid() const {
    glm::vec3 centroid;

    for (unsigned int c = 0; c < this->positions.size(); ++c) {
      for (unsigned int d = 0; d < 3; ++d) {
        centroid[d] += this->positions[c][d] / this->positions.size();
      }
    }

    return centroid;
  }
  
};

template <unsigned int C>
class Boundable<FixedPolygon<C>> : 
  public BoundableBase<FixedPolygon<C>> {
  
  using FixedPolygon<C>::FixedPolygon;

  public:
  
  virtual Bound GetBound() const {
    Bound bound;

    for (unsigned int d = 0; d < 3; ++d) {
      bound.min(d) = this->positions[0][d];
    }
    for (unsigned int d = 0; d < 3; ++d) {
      bound.max(d) = this->positions[0][d];
    }

    for (unsigned int c = 1; c < C; ++c) {
      for (unsigned int d = 0; d < 3; ++d) {
        if (this->positions[c][d] < bound.min(d)) bound.min(d) = this->positions[c][d];
      }
      for (unsigned int d = 0; d < 3; ++d) {
        if (this->positions[c][d] > bound.max(d)) bound.max(d) = this->positions[c][d];
      }
    }

    return bound;
  }

  virtual glm::vec3 GetCentroid() const {
    glm::vec3 centroid;

    for (unsigned int c = 0; c < C; ++c) {
      for (unsigned int d = 0; d < 3; ++d) {
        centroid[d] += this->positions[c][d] / C;
      }
    }

    return centroid;
  }
  
};