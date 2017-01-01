
#pragma once

#include <glm/vec3.hpp>
#include "bound.h"

class BoundableInterface {
public:

  virtual Bound GetBound() const = 0;
  virtual glm::vec3 GetCentroid() const = 0;
};

template <typename G>
class BoundableBase : 
  public G,
  public virtual BoundableInterface {
  
};