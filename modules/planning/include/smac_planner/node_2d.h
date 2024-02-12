#pragma once

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
    Coordinates() {}
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

  inline std::uint32_t get_index() { return index_; }

private:
  double cell_cost_;
  double accumulated_cost_;
  std::uint32_t index_;
  bool was_visited_;
  bool is_queued_;
};

}  // namespace smac_planner
}  // namespace planning