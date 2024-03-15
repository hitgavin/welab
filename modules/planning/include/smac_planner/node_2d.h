#pragma once

#include "core/logger/logger.h"
#include "smac_planner/collision_checker.h"

#include <cstdint>
#include <memory>
#include <vector>

namespace planning {
namespace smac_planner {

/**
 * @class Node2D
 * @brief Node2D implementation for graph
 *
 */
class Node2D {
public:
  typedef Node2D* NodePtr;
  typedef std::unique_ptr<std::vector<Node2D>> Graph;
  typedef std::vector<NodePtr> NodeVector;

  /**
   * @struct Coordinates
   * @brief Node2D implementation of coordinate structure.
   *
   */
  struct Coordinates {
    Coordinates() : x_(0.0), y_(0.0) {}
    Coordinates(double x, double y) : x_(x), y_(y) {}

    double x_;
    double y_;
  };
  typedef std::vector<Coordinates> CoordinateVector;

  /**
   * @brief A constructor for Node2D
   *
   * @param index The index of this node for self-reference
   */
  explicit Node2D(std::uint32_t index);

  /**
   * @brief Destructor for Node2D
   */
  ~Node2D();

  /**
   * @brief operator== for comparisons
   *
   * @param rhs Right hand side node reference
   * @return If cell indicies are equal.
   */
  bool operator==(const Node2D& rhs) { return index_ == rhs.index_; }

  /**
   * @brief Reset method for new search.
   */
  void reset();

  /**
   * @brief Get the accumulated cost at this node.
   *
   * @return accumulated cost
   */
  inline double get_accumulated_cost() { return accumulated_cost_; }
  /**
   * @brief Set the accumulated cost at this node.
   *
   * @param cost accumulated cost.
   */
  inline void set_accumulated_cost(double cost) { accumulated_cost_ = cost; }

  /**
   * @brief Get traversal cost from this node to child node
   *
   * @param child Node pointer to this node's child
   * @return traversal cost
   */
  double get_traversal_cost(const NodePtr& child);

  /**
   * @brief Get the costmap cost at this node.
   *
   * @return costmap cost.
   */
  inline double get_cost() { return cell_cost_; }
  /**
   * @brief Set the costmap cost at this node.
   *
   * @param cost costmap cost.
   */
  inline void set_cost(double cost) { cell_cost_ = cost; }

  /**
   * @brief Gets if cell has been visited in search.
   *
   * @return If cell was visited.
   */
  inline bool was_visited() { return was_visited_; }
  /**
   * @brief Set cell has been visited in search.
   */
  inline void set_visited() {
    was_visited_ = true;
    is_queued_ = false;
  }
  /**
   * @brief Gets if cell is currently queued in search.
   *
   * @return If cell was queued.
   */
  inline bool is_queued() { return is_queued_; }
  /**
   * @brief Set cell is currently queued in search.
   */
  inline void set_queued() { is_queued_ = true; }
  /**
   * @brief Get the cell index
   * @return cell index
   */
  inline std::uint32_t get_index() { return index_; }
  /**
   * @brief Chcke if this node is valid
   *
   * @param[in] traverse_unknown If we can explore unknown nodes on the graph
   * @param[in] collision_checker Pointer to collision checker object
   * @return Whether this node is valid and collision free.
   */
  bool is_node_valid(
      bool traverse_unknown,
      const std::shared_ptr<GridCollisionChecker>& collision_checker);

  /**
   * @brief Retrieve all valid neighbors of a node.
   *
   * @param neighbor_getter Functor for state validity checking
   * @param collision_checker Collision checker to use
   * @param traverse_unknown If unknown costs are valid to traverse
   * @param neighbors Vector of neighbors to be filled
   */
  void get_neighbors(
      std::function<bool(std::uint32_t, NodePtr&)>& neighbor_getter,
      std::shared_ptr<GridCollisionChecker>& collision_checker,
      bool traverse_unknown, NodeVector& neighbors);

  /**
   * @brief backtrace the path based on this node.
   *
   * @param path Reference to a vector of indicies of generated path
   * @return whether the path was able to be backtraced
   */
  bool backtrace_path(CoordinateVector& path);

  /**
   * @brief Get the coordinates of point
   *
   * @param index Index of point
   * @param width width of costmap
   * @param angles angle bins to use (must be 1)
   * @return Coordinates of point
   */
  static inline Coordinates get_coords(std::uint32_t index, std::uint32_t width,
                                       std::uint32_t angles) {
    if (angles != 1) {
      logger()->error(
          "Node type Node2D does not have a valid angle quantization.");
    }

    std::uint32_t x = index % width;
    std::uint32_t y = index / width;

    return Coordinates(x, y);
  }

  /**
   * @brief Get the coordinates of point
   *
   * @param index Index of point
   * @return Coordinates of point
   */
  static inline Coordinates get_coords(const std::uint32_t index) {
    const std::uint32_t size_x = neighbors_grid_offsets[3];
    std::uint32_t x = index % size_x;
    std::uint32_t y = index / size_x;
    return Coordinates(x, y);
  }

  /**
   * @brief Get cost of heuristic of node
   *
   * @param node_coords Node index current
   * @param goal_coords Node index of new
   * @return Heuristic cost between toe nodes
   */
  static double get_heuristic_cost(const Coordinates& node_coords,
                                   const Coordinates& goal_coords);

  static inline std::uint32_t get_index(std::uint32_t x, std::uint32_t y,
                                        std::uint32_t width) {
    return x + y * width;
  }

  static double cost_travel_multiplier;
  static std::vector<int> neighbors_grid_offsets;

  Node2D* parent_;

private:
  double cell_cost_;
  double accumulated_cost_;
  std::uint32_t index_;
  bool was_visited_;
  bool is_queued_;
};

}  // namespace smac_planner
}  // namespace planning