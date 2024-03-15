#include "smac_planner/node_2d.h"

#include <costmap/cost_values.h>
#include <cmath>
#include <limits>

using map::costmap_2d::MAX_NON_OBSTACLE;

namespace planning {
namespace smac_planner {

double Node2D::cost_travel_multiplier = 2.0;
std::vector<int> Node2D::neighbors_grid_offsets;

Node2D::Node2D(std::uint32_t index)
    : parent_(nullptr),
      cell_cost_(std::numeric_limits<double>::quiet_NaN()),
      accumulated_cost_(std::numeric_limits<double>::max()),
      index_(index),
      was_visited_(false),
      is_queued_(false) {}

double Node2D::get_traversal_cost(const NodePtr& child) {
  double normalized_cost = child->get_cost() / MAX_NON_OBSTACLE;
  const Coordinates A = get_coords(child->get_index());
  const Coordinates B = get_coords(get_index());
  double dx = A.x_ - B.x_;
  double dy = A.y_ - B.y_;
  static double sqrt_2 = std::sqrt(2.0);

  // If a diagonal move, travel cost is sqrt(2) not 1.0.
  if ((dx * dx) + (dy * dy) > 1.05) {
    return sqrt_2 * (1.0 + cost_travel_multiplier * normalized_cost);
  }

  // Length = 1.0
  return 1.0 + cost_travel_multiplier * normalized_cost;
}

bool Node2D::is_node_valid(
    bool traverse_unknown,
    const std::shared_ptr<GridCollisionChecker>& collision_checker) {
  if (collision_checker->in_collision(get_index(), traverse_unknown)) {
    return false;
  }
  return true;
}

void Node2D::get_neighbors(
    std::function<bool(std::uint32_t, NodePtr&)>& neighbor_getter,
    std::shared_ptr<GridCollisionChecker>& collision_checker,
    bool traverse_unknown, NodeVector& neighbors) {
  // clang-format off
  // Irritatingly, the order here matters, If you start in free space and then
  // expand 8-connected, the first set of neighbors will be all cost 1.0. Then
  // its expansion will all be 2 * 1.0 but now multiple nodes are touching that
  // node so the last cell to update the back pointer wins. Thusly, the ordering
  // ends with the cardinal directions for both sets such that behavior is
  // consistent in large free spaces between them.
  // 100  50   0
  // 100  50   50
  // 100  100  100 where lower-middle '100' is visited with same cost by both
  // bottom '50' nodes. Therefore, it is valuable to have some low-potential
  // across the entire map rather than a small inflation around the obstacles
  // clang-format on

  std::uint32_t index;
  NodePtr neighbor = nullptr;
  std::uint32_t node_i = get_index();
  const Coordinates parent = get_coords(get_index());
  Coordinates child;

  for (std::uint32_t i = 0; i != neighbors_grid_offsets.size(); ++i) {
    index = node_i + neighbors_grid_offsets[i];

    // Check for wrap around conditions
    child = get_coords(index);
    if (std::fabs(parent.x_ - child.x_) > 1 ||
        std::fabs(parent.y_ - child.y_) > 1) {
      continue;
    }

    if (neighbor_getter(index, neighbor)) {
      if (neighbor->is_node_valid(traverse_unknown, collision_checker) &&
          !neighbor->was_visited()) {
        neighbors.push_back(neighbor);
      }
    }
  }
}

bool Node2D::backtrace_path(CoordinateVector& path) {
  if (!this->parent_) {
    return false;
  }

  NodePtr current_node = this;
  while (current_node->parent_) {
    path.push_back(Node2D::get_coords(current_node->get_index()));
    current_node = current_node->parent_;
  }

  // add the start pose
  path.push_back(Node2D::get_coords(current_node->get_index()));

  return true;
}

double Node2D::get_heuristic_cost(const Coordinates& node_coords,
                                  const Coordinates& goal_coords) {
  // Using Moore distance as it more accurately represents the distances even a
  // Van Neumann neighborhood robot can navigate.
  auto dx = goal_coords.x_ - node_coords.x_;
  auto dy = goal_coords.y_ - node_coords.y_;
  return std::sqrt(dx * dx + dy * dy);
}

}  // namespace smac_planner
}  // namespace planning