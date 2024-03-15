#pragma once

#include <cstdint>

namespace planning {
namespace smac_planner {

template <typename NodeT>
class NodeBasic {
public:
  explicit NodeBasic(std::uint32_t index)
      : index_(index), graph_node_ptr_(nullptr) {}

  /**
   * @brief Take a NodeBasic and populate it with any necessary state
   * cached in the queue for NodeT.
   * @param node NodeT ptr to populate meta data into NodeBasic
   */
  void populate_search_node(NodeT *node);

  void process_search_node();

  typename NodeT::Coordinates pose_;
  NodeT *graph_node_ptr_;
  std::uint32_t index_;
  std::uint32_t motion_index_;
};

}  // namespace smac_planner
}  // namespace planning