
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

    for (unsigned int c = 0; c < this->positions.size(); ++c) {
      bound.merge(this->positions[c]);
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
    
    for (unsigned int c = 0; c < C; ++c) { 
      bound.merge(this->positions[c]);
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