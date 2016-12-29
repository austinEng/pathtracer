
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
      bound.min(d) = this->points[0][d];
    }
    for (unsigned int d = 0; d < 3; ++d) {
      bound.max(d) = this->points[0][d];
    }

    for (unsigned int c = 1; c < this->points.size(); ++c) {
      for (unsigned int d = 0; d < 3; ++d) {
        if (this->points[c][d] < bound.min(d)) bound.min(d) = this->points[c][d];
      }
      for (unsigned int d = 0; d < 3; ++d) {
        if (this->points[c][d] > bound.max(d)) bound.max(d) = this->points[c][d];
      }
    }

    return bound;
  }

  virtual glm::vec3 GetCentroid() const {
    glm::vec3 centroid;

    for (unsigned int c = 0; c < this->points.size(); ++c) {
      for (unsigned int d = 0; d < 3; ++d) {
        centroid[d] += this->points[c][d] / this->points.size();
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
      bound.min(d) = this->points[0][d];
    }
    for (unsigned int d = 0; d < 3; ++d) {
      bound.max(d) = this->points[0][d];
    }

    for (unsigned int c = 1; c < C; ++c) {
      for (unsigned int d = 0; d < 3; ++d) {
        if (this->points[c][d] < bound.min(d)) bound.min(d) = this->points[c][d];
      }
      for (unsigned int d = 0; d < 3; ++d) {
        if (this->points[c][d] > bound.max(d)) bound.max(d) = this->points[c][d];
      }
    }

    return bound;
  }

  virtual glm::vec3 GetCentroid() const {
    glm::vec3 centroid;

    for (unsigned int c = 0; c < C; ++c) {
      for (unsigned int d = 0; d < 3; ++d) {
        centroid[d] += this->points[c][d] / C;
      }
    }

    return centroid;
  }
  
};