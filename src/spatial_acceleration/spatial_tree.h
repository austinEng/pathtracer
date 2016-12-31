
#pragma once

#include "bound.h"
#include "boundable.h"
#include <vector>
#include <deque>
#include <memory>

template <unsigned int B, unsigned int L>
class SpatialTree {
public:

  typedef Bound bound_t;
  typedef BoundableInterface object_t;

  class alignas(64) Node {
    public:
    std::shared_ptr<object_t> objects[L];
    union {
      Node* children[B];
      unsigned int childIndices[B];
    };
    bool leaf;
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

  };

  class alignas(64) NodeGroup {
    public:
    BoundSOA<B> bounds;
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

      unsigned int indices[B];
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
  }

protected:
  int nodeCount;
  Node root;
  std::vector<Node> nodes; // this should probably be cache-aligned. probably use a memory arena too

};