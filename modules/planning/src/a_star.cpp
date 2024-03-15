#include "smac_planner/a_star.h"

#include "costmap/cost_values.h"
#include "smac_planner/node_2d.h"

#include "core/logger/logger.h"
#include "smac_planner/node_basic.h"

#include <limits>
#include <tuple>

namespace planning {
namespace smac_planner {

template <typename NodeT>
AStarAlgorithm<NodeT>::AStarAlgorithm()
    : start_(nullptr),
      goal_(nullptr),
      goal_coordinates_(Coordinates()),
      best_heuristic_node_(std::numeric_limits<double>::max(), 0),
      tolerance_(0.0),
      traverse_unknown_(true),
      max_iterations_(0),
      max_planning_time_(0.0) {
  graph_.reserve(100000);
}

template <typename NodeT>
void AStarAlgorithm<NodeT>::initialize(bool allow_unknown,
                                       std::uint32_t max_iterations,
                                       std::uint32_t max_on_approach_iterations,
                                       double max_planning_time,
                                       double lookup_table_size,
                                       std::uint32_t dim_3_size) {
  traverse_unknown_ = allow_unknown;
  max_iterations_ = max_iterations;
  max_on_approach_iterations_ = max_on_approach_iterations;
  max_planning_time_ = max_planning_time;
  dim3_size_ = dim_3_size;

  std::ignore = lookup_table_size;
}

template <>
void AStarAlgorithm<Node2D>::initialize(
    bool allow_unknown, std::uint32_t max_iterations,
    std::uint32_t max_on_approach_iterations, double max_planning_time,
    double lookup_table_size, std::uint32_t dim_3_size) {
  traverse_unknown_ = allow_unknown;
  max_iterations_ = max_iterations;
  max_on_approach_iterations_ = max_on_approach_iterations;
  max_planning_time_ = max_planning_time;
  dim3_size_ = dim_3_size;

  if (dim_3_size != 1) {
    logger()->error(
        "Node type Node2D cannot be given non-1 dim 3 quantization.");
  }

  dim3_size_ = 1;
  std::ignore = lookup_table_size;
}

template <typename NodeT>
bool AStarAlgorithm<NodeT>::create_path(CoordinateVector &path,
                                        std::uint32_t &iterations,
                                        const double &tolerance) {
  tolerance_ = tolerance;
  best_heuristic_node_ = {std::numeric_limits<double>::max(), 0};
  clear_queue();

  if (!are_inputs_valid()) {
    return false;
  }

  // 0) Add start point to the open set
  add_node(0.0, get_start());
  get_start()->set_accumulated_cost(0.0);

  // Preallocate all variables
  NodePtr current_node = nullptr;
  NodePtr neighbor = nullptr;
  // NodePtr expansion_result = nullptr;
  double g_cost = 0.0;
  NodeVector neighbors;
  std::uint32_t approach_iterations = 0;
  NeighborIterator neighbor_iterator;
  // int analytic_iterations = 0;
  // int closest_distance = std::numeric_limits<int>::max();

  // Given an index, return a node ptr reference if its collision-free and valid
  const std::uint32_t max_index = get_size_x() * get_size_y() * get_size_dim3();
  NodeGetter neighbor_getter = [&, this](std::uint32_t index,
                                         NodePtr &neighbor_rtn) -> bool {
    if (index >= max_index) {
      return false;
    }

    neighbor_rtn = add_to_graph(index);
    return true;
  };

  while (iterations < get_max_iterations() && !queue_.empty()) {
    // 1) Pick Nbest from O s.t. min(f(Nbest)), remove from queue
    current_node = get_next_node();

    // We allow for nodes to be queued multiple times in case shoter paths
    // result in it, but we can visit only once
    if (current_node->was_visited()) {
      continue;
    }

    iterations++;
    // 2) Mark Nbest as visited
    current_node->set_visited();
    // 2.1) Use an analytic expansion (if available) to generate a path

    // 3) Check if we're at the goal, backtrace if required
    if (is_goal(current_node)) {
      return current_node->backtrace_path(path);
    } else if (best_heuristic_node_.first < get_tolerance_heuristic()) {
      approach_iterations++;
      if (approach_iterations >= get_on_approach_max_iterations()) {
        return graph_.at(best_heuristic_node_.second).backtrace_path(path);
      }
    }

    // 4) Expand neighbors of Nbest not visited
    neighbors.clear();
    current_node->get_neighbors(neighbor_getter, collision_checker_,
                                traverse_unknown_, neighbors);

    for (neighbor_iterator = neighbors.begin();
         neighbor_iterator != neighbors.end(); ++neighbor_iterator) {
      neighbor = *neighbor_iterator;
      // 4.1) Compute the cost to go to this node
      g_cost = current_node->get_accumulated_cost() +
               current_node->get_traversal_cost(neighbor);

      // 4.2) If this is a lower cost than prior, we set this as the new cost
      // and new approach
      if (g_cost < neighbor->get_accumulated_cost()) {
        neighbor->set_accumulated_cost(g_cost);
        neighbor->parent_ = current_node;

        // 4.3) Add to queue with heuristic cost
        add_node(g_cost + get_heuristic_cost(neighbor), neighbor);
      }
    }
  }

  if (best_heuristic_node_.first < get_tolerance_heuristic()) {
    // If we run out of search options, return the path that is closest, if
    // within tolerance.
    return graph_.at(best_heuristic_node_.second).backtrace_path(path);
  }

  return false;
}

template <typename NodeT>
void AStarAlgorithm<NodeT>::set_collision_checker(
    std::shared_ptr<GridCollisionChecker> &collision_checker) {
  collision_checker_ = collision_checker;
  costmap_ = collision_checker_->get_costmap();

  std::uint32_t x_size = costmap_->get_size_in_cells_x();
  std::uint32_t y_size = costmap_->get_size_in_cells_y();

  clear_graph();
}

template <typename NodeT>
bool AStarAlgorithm<NodeT>::set_start(std::uint32_t mx, std::uint32_t my,
                                      std::uint32_t dim_3) {
  std::ignore = mx;
  std::ignore = my;
  std::ignore = dim_3;
  return true;
}

template <>
bool AStarAlgorithm<Node2D>::set_start(std::uint32_t mx, std::uint32_t my,
                                       std::uint32_t dim_3) {
  if (dim_3 != 0) {
    logger()->error(
        "Node type Node2D cannot be given non-zero starting dim 3.");
  }

  start_ = add_to_graph(Node2D::get_index(mx, my, get_size_x()));

  return true;
}

template <typename NodeT>
bool AStarAlgorithm<NodeT>::set_goal(std::uint32_t mx, std::uint32_t my,
                                     std::uint32_t dim_3) {
  std::ignore = mx;
  std::ignore = my;
  std::ignore = dim_3;
  return true;
}

template <>
bool AStarAlgorithm<Node2D>::set_goal(std::uint32_t mx, std::uint32_t my,
                                      std::uint32_t dim_3) {
  if (dim_3 != 0) {
    logger()->error("Node type Node2D cannot be given non-zero goal dim 3.");
    return false;
  }
  goal_ = add_to_graph(Node2D::get_index(mx, my, get_size_x()));
  goal_coordinates_ = Node2D::Coordinates(mx, my);

  return true;
}

template <typename NodeT>
typename AStarAlgorithm<NodeT>::NodePtr AStarAlgorithm<NodeT>::get_start() {
  return start_;
}

template <typename NodeT>
typename AStarAlgorithm<NodeT>::NodePtr AStarAlgorithm<NodeT>::get_goal() {
  return goal_;
}

template <typename NodeT>
std::uint32_t AStarAlgorithm<NodeT>::get_on_approach_max_iterations() {
  return max_on_approach_iterations_;
}

template <typename NodeT>
std::uint32_t AStarAlgorithm<NodeT>::get_max_iterations() {
  return max_iterations_;
}

template <typename NodeT>
double &AStarAlgorithm<NodeT>::get_tolerance_heuristic() {
  return tolerance_;
}

template <typename NodeT>
typename AStarAlgorithm<NodeT>::NodePtr AStarAlgorithm<NodeT>::get_next_node() {
  NodeBasic<NodeT> node = queue_.top().second;
  queue_.pop();
  node.process_search_node();
  return node.graph_node_ptr_;
}

template <typename NodeT>
void AStarAlgorithm<NodeT>::add_node(double cost, NodePtr node) {
  NodeBasic<NodeT> queued_node(node->get_index());
  queued_node.populate_search_node(node);
  queue_.emplace(cost, queued_node);
  node->set_queued();
}

template <typename NodeT>
typename AStarAlgorithm<NodeT>::NodePtr AStarAlgorithm<NodeT>::add_to_graph(
    const std::uint32_t &index) {
  auto iter = graph_.find(index);
  if (iter != graph_.end()) {
    return &(iter->second);
  }

  return &(graph_.emplace(index, NodeT(index)).first->second);
}

template <typename NodeT>
bool AStarAlgorithm<NodeT>::is_goal(NodePtr node) {
  return node == get_goal();
}

template <typename NodeT>
double AStarAlgorithm<NodeT>::get_heuristic_cost(const NodePtr &node) {
  const Coordinates node_coords =
      NodeT::get_coords(node->get_index(), get_size_x(), get_size_dim3());
  double heuristic = NodeT::get_heuristic_cost(node_coords, goal_coordinates_);

  if (heuristic < best_heuristic_node_.first) {
    best_heuristic_node_ = {heuristic, node->get_index()};
  }

  return heuristic;
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

  if (get_tolerance_heuristic() < 0.001 &&
      !goal_->is_node_valid(traverse_unknown_, collision_checker_)) {
    logger()->error("Goal was in lethal cost");
    return false;
  }

  clear_start();

  return true;
}

template <typename NodeT>
void AStarAlgorithm<NodeT>::clear_queue() {
  NodeQueue q;
  std::swap(queue_, q);
}

template <typename NodeT>
void AStarAlgorithm<NodeT>::clear_graph() {
  Graph g;
  std::swap(graph_, g);
  graph_.reserve(100000);
}

template <typename NodeT>
void AStarAlgorithm<NodeT>::clear_start() {
  auto coords = NodeT::get_coords(
      start_->get_index(), costmap_->get_size_in_cells_x(), get_size_dim3());
  costmap_->set_cost(coords.x_, coords.y_, map::costmap_2d::FREE_SPACE);
}

// Instantiate algorithm for the supported template types
template class AStarAlgorithm<Node2D>;

}  // namespace smac_planner
}  // namespace planning