
#pragma once

#include "bound.h"
#include "boundable.h"
#include <vector>
#include <memory>

template <int D, typename T>
class SpatialTree {
public:

  typedef Bound<D, T> bound_t;
  typedef BoundableInterface<D, T> object_t;

  class alignas(64) Node {
    public:
    std::shared_ptr<object_t> object = nullptr;
    Node* children[2] = {nullptr, nullptr};
    bound_t bound;

    Node(Node* left, Node* right) : children {left, right} {
      bound = Bound<D, T>::Union(
        left != nullptr ? left->bound : Bound<D, T>(), 
        right != nullptr ? right->bound : Bound<D, T>()
      );
    }

    Node(std::shared_ptr<object_t> &obj) : object(obj) {
      if (obj != nullptr) {
        bound = obj->GetBound();
      }
    }
  };

  virtual Node* internalBuild(std::vector<std::shared_ptr<object_t>> &objects) = 0;
  void build(std::vector<std::shared_ptr<object_t>> &objects) {
    internalBuild(objects);
  }

protected:
  std::vector<Node> nodes; // this should probably be cache-aligned. probably use a memory arena too

};