
#pragma once

#include <geometry/geometry.h>
#include "boundable.h"

template <typename G>
class Boundable { };

template <unsigned int D, typename T>
class Boundable<Polygon<D, T>> : public BoundableBase<Polygon<D, T>> {

  using Polygon<D,T>::Polygon;

  public:
  
  virtual Bound<D, T> GetBound() const {
    Bound<D, T> bound;

    for (unsigned int d = 0; d < D; ++d) {
      bound.min(d) = this->points[0][d];
    }
    for (unsigned int d = 0; d < D; ++d) {
      bound.max(d) = this->points[0][d];
    }

    for (unsigned int c = 1; c < this->points.size(); ++c) {
      for (unsigned int d = 0; d < D; ++d) {
        if (this->points[c][d] < bound.min(d)) bound.min(d) = this->points[c][d];
      }
      for (unsigned int d = 0; d < D; ++d) {
        if (this->points[c][d] > bound.max(d)) bound.max(d) = this->points[c][d];
      }
    }

    return bound;
  }

  virtual Vector<D, T> GetCentroid() const {
    Vector<D, T> centroid;

    for (unsigned int c = 0; c < this->points.size(); ++c) {
      for (unsigned int d = 0; d < D; ++d) {
        centroid[d] += this->points[c][d] / this->points.size();
      }
    }

    return centroid;
  }
  
};

template <unsigned int C, unsigned int D, typename T>
class Boundable<FixedPolygon<C, D, T>> : 
  public BoundableBase<FixedPolygon<C, D, T>> {
  
  using FixedPolygon<C, D, T>::FixedPolygon;

  public:
  
  virtual Bound<D, T> GetBound() const {
    Bound<D, T> bound;

    for (unsigned int d = 0; d < D; ++d) {
      bound.min(d) = this->points[0][d];
    }
    for (unsigned int d = 0; d < D; ++d) {
      bound.max(d) = this->points[0][d];
    }

    for (unsigned int c = 1; c < C; ++c) {
      for (unsigned int d = 0; d < D; ++d) {
        if (this->points[c][d] < bound.min(d)) bound.min(d) = this->points[c][d];
      }
      for (unsigned int d = 0; d < D; ++d) {
        if (this->points[c][d] > bound.max(d)) bound.max(d) = this->points[c][d];
      }
    }

    return bound;
  }

  virtual Vector<D, T> GetCentroid() const {
    Vector<D, T> centroid;

    for (unsigned int c = 0; c < C; ++c) {
      for (unsigned int d = 0; d < D; ++d) {
        centroid[d] += this->points[c][d] / C;
      }
    }

    return centroid;
  }
  
};