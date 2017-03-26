
#pragma once

namespace accel {

template <unsigned int B, unsigned int L>
struct BranchingFactor {
  static const int NODE = B;
  static const int LEAF = L;
};

template <typename N, typename P>
struct NodeStorage {
  typedef N node_t;
  typedef P prim_t;
};

}