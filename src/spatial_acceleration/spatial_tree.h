
#pragma once

#include "bound.h"
#include "boundable.h"
#include <vector>
#include <memory>

template <unsigned int B, unsigned int L>
class SpatialTree {
public:

  typedef Bound bound_t;
  typedef BoundableInterface object_t;

  class alignas(64) Node {
    public:
    std::shared_ptr<object_t> objects[L];
    Node* children[B];
    bound_t bound;

    Node() {
      std::memset(this->children, 0, sizeof(this->children));
      std::memset(this->objects, 0, sizeof(this->objects));
    }

    void setChildren(Node* children[B]) {
      for (unsigned int i = 0; i < B; ++i) {
        this->children[i] = children[i];
      }
      bound = bound_t();
      for (unsigned int i = 0; i < B; ++i) {
        if (this->children[i] != nullptr) {
          bound.merge(this->children[i]->bound);
        }
      }
    }

    void setObjects(std::shared_ptr<object_t> objects[L]) {
      for (unsigned int i = 0; i < L; ++i) {
        this->objects[i] = objects[i];
      }
      bound = bound_t();
      for (unsigned int i = 0; i < L; ++i) {
        if (this->objects[i] != nullptr) {
          bound.merge(this->objects[i]->GetBound());
        }
      }
    }

    // Node(std::shared_ptr<object_t> &obj) : object(obj) {
    //   std::memset(this->children, 0, sizeof(this->children));
    //   if (obj != nullptr) {
    //     bound = obj->GetBound();
    //   }
    // }
  };

  virtual Node* internalBuild(std::vector<std::shared_ptr<object_t>> &objects) = 0;
  void build(std::vector<std::shared_ptr<object_t>> &objects) {
    internalBuild(objects);
  }

protected:
  std::vector<Node> nodes; // this should probably be cache-aligned. probably use a memory arena too

};