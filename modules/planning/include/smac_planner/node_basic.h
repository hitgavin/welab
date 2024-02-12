#pragma once

#include <cstdint>

namespace planning {
namespace smac_planner {

template <typename NodeT>
class NodeBasic {
public:
  explicit NodeBasic(std::uint32_t index)
      : index_(index), graph_node_ptr_(nullptr) {}

  typename NodeT::Coordinates pose_;
  NodeT *graph_node_ptr_;
  std::uint32_t index_;
  std::uint32_t motion_index_;
};

}  // namespace smac_planner
}  // namespace planning