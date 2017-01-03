
#pragma once

#include "bound.h"
#include "boundable.h"
#include <vector>
#include <deque>
#include <memory>
#include <ae_core/simd/types.h>

template <unsigned int B, unsigned int L>
class SpatialTree {
public:

  typedef Bound bound_t;
  typedef BoundableInterface object_t;
  // typedef BoundableBase<Primitive> object_t;

  class alignas(64) Node {
    public:
    std::shared_ptr<object_t> objects[L] = { nullptr };
    union {
      Node* children[B] = { nullptr };
      int childIndices[B];
    };
    bool leaf;
    bound_t bound;

    Node() { }

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

  };

  class alignas(64) NodeGroup {
    public:
    std::shared_ptr<object_t> objects[B*L] = { nullptr };
    union {
      NodeGroup* children[B] = { nullptr };
      int childIndices[B];
    };
    BoundSoA<B> bound;
    SIMD::Float<B> leaf;
  };

  virtual Node* internalBuild(std::vector<std::shared_ptr<object_t>> &objects, Node* arena) = 0;

  void build(std::vector<std::shared_ptr<object_t>> &objects) {
    Node* arena = (Node*) malloc(sizeof(Node) * objects.size() * 2);
    Node* root = internalBuild(objects, arena);
    memcpy(&this->root, root, sizeof(Node));

    this->nodes.clear();
    this->nodes.reserve(this->nodeCount - 1);
    std::deque<Node*> queue;
    queue.push_back(&this->root);
    unsigned int idx = 0;
    while(queue.size() > 0) {
      Node* node = queue.front();
      queue.pop_front();

      int indices[B];
      if (!node->leaf) {
        for (unsigned int i = 0; i < B; ++i) {
          if (node->children[i] != nullptr) {
            indices[i] = idx++;
            this->nodes.emplace_back();
            Node* newNode = &this->nodes.back();
            memcpy(newNode, node->children[i], sizeof(Node));
            queue.push_back(newNode);
            
          } else {
            idx++;
            indices[i] = -1;
            this->nodes.emplace_back();
          }
        }
      }
      for (unsigned int i = 0; i < B; ++i) {
        node->childIndices[i] = indices[i];
      }
    }
    std::cout << this->nodes.size() << " nodes" << std::endl;
    free(arena);
    std::cout << this->root.bound << std::endl;
  }

  Node root;
  int nodeCount;
  std::vector<Node> nodes;

};