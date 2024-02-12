#include "smac_planner/a_star.h"

#include "smac_planner/node_2d.h"

#include "core/logger/logger.h"

#include <limits>

namespace planning {
namespace smac_planner {

template <typename NodeT>
AStarAlgorithm<NodeT>::AStarAlgorithm()
    : start_(nullptr),
      goal_(nullptr),
      best_heuristic_node_(std::numeric_limits<double>::max(), 0),
      tolerance_(0.0) {
  graph_.reserve(100000);
}

template <typename NodeT>
bool AStarAlgorithm<NodeT>::create_path(CoordinateVector &path,
                                        std::uint32_t &num_iterations,
                                        const double &tolerance) {
  tolerance_ = tolerance;
  best_heuristic_node_ = {std::numeric_limits<double>::max(), 0};
  clear_queue();

  return true;
}

template <typename NodeT>
void AStarAlgorithm<NodeT>::set_start(const std::uint32_t &mx,
                                      const std::uint32_t &my,
                                      const std::uint32_t &dim_3) {}

template <typename NodeT>
void AStarAlgorithm<NodeT>::set_goal(const std::uint32_t &mx,
                                     const std::uint32_t &my,
                                     const std::uint32_t &dim_3) {}

template <typename NodeT>
typename AStarAlgorithm<NodeT>::NodePtr AStarAlgorithm<NodeT>::get_start() {
  return start_;
}

template <typename NodeT>
typename AStarAlgorithm<NodeT>::NodePtr AStarAlgorithm<NodeT>::get_goal() {
  return goal_;
}

template <typename NodeT>
bool AStarAlgorithm<NodeT>::are_inputs_valid() {
  if (graph_.empty()) {
    logger()->error("Failed to compute path, no map given.");
    return false;
  }

  if (!start_ || !goal_) {
    logger()->error("Failed to compute path, no valid start or goal given.");
    return false;
  }

  return true;
}

template <typename NodeT>
void AStarAlgorithm<NodeT>::clear_queue() {
  NodeQueue q;
  std::swap(queue_, q);
}

// Instantiate algorithm for the supported template types
template class AStarAlgorithm<Node2D>;

}  // namespace smac_planner
}  // namespace planning