#include "smac_planner/node_basic.h"
#include "smac_planner/node_2d.h"

namespace planning {
namespace smac_planner {

template <>
void NodeBasic<Node2D>::populate_search_node(Node2D *node) {
  graph_node_ptr_ = node;
}

template <typename Node2D>
void NodeBasic<Node2D>::process_search_node() {}

template class NodeBasic<Node2D>;

}  // namespace smac_planner
}  // namespace planning