
#pragma once

#include <geometry/geometry.h>
#include "boundable.h"

template <typename G>
class Bounded { };

template <unsigned int D, typename T>
class Bounded<Polygon<D, T>> : 
  public Polygon<D, T>,
  public Boundable<Polygon<D, T>::dim, typename Polygon<D, T>::val_t> {
  
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

  virtual Point<D, T> GetCentroid() const {
    Point<D, T> centroid;

    for (unsigned int c = 0; c < this->points.size(); ++c) {
      for (unsigned int d = 0; d < D; ++d) {
        centroid[d] += this->points[c][d] / this->points.size();
      }
    }

    return centroid;
  }
  
};

template <unsigned int C, unsigned int D, typename T>
class Bounded<FixedPolygon<C, D, T>> : 
  public FixedPolygon<C, D, T>,
  public Boundable<FixedPolygon<C, D, T>::dim, typename FixedPolygon<C, D, T>::val_t> {
  
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

  virtual Point<D, T> GetCentroid() const {
    Point<D, T> centroid;

    for (unsigned int c = 0; c < C; ++c) {
      for (unsigned int d = 0; d < D; ++d) {
        centroid[d] += this->points[c][d] / C;
      }
    }

    return centroid;
  }
  
};

template <unsigned int D, typename T>
class Bounded<Triangle<D,T>> :
  public Triangle<D, T>,
  public Bounded<FixedPolygon<3,D,T>> {
};

template <unsigned int D, typename T>
class Bounded<Quad<D,T>> :
  public Quad<D, T>,
  public Bounded<FixedPolygon<4,D,T>> {
};