#pragma once

/**
 * @defgroup planning_group planning module
 * @brief The main namespace group for map module.
 *
 * This group contains all kinds of planners.
 *
 */

/**
 * @defgroup smac_planner_group smac_planner submodule
 * @ingroup planning_group
 * @brief smac planner
 *
 */

#include "costmap/costmap_2d.h"
#include "node_basic.h"
#include "smac_planner/collision_checker.h"
#include "smac_planner_types.h"

#include <robin_hood.h>

#include <cstdint>
#include <functional>
#include <queue>
#include <utility>

namespace planning {
namespace smac_planner {

template <typename NodeT>
class AStarAlgorithm {
public:
  typedef NodeT *NodePtr;
  typedef std::vector<NodePtr> NodeVector;
  typedef std::pair<double, NodeBasic<NodeT>> NodeElement;
  typedef typename NodeVector::iterator NeighborIterator;
  typedef typename NodeT::Coordinates Coordinates;
  typedef typename NodeT::CoordinateVector CoordinateVector;
  typedef robin_hood::unordered_node_map<std::uint32_t, NodeT> Graph;
  typedef std::function<bool(std::uint32_t, NodePtr &)> NodeGetter;

  struct NodeComparator {
    bool operator()(const NodeElement &a, const NodeElement &b) {
      return a.first > b.first;
    }
  };

  typedef std::priority_queue<NodeElement, std::vector<NodeElement>,
                              NodeComparator>
      NodeQueue;

  AStarAlgorithm();

  /**
   * @brief Initialization of the planner with defaults
   *
   * @param allow_unknown Allow search in unknown space, good for navigation
   * while mapping
   * @param max_iterations Maximum number of iterations to use while expanding
   * search
   * @param max_on_approach_iterations Maximum number of iterations before
   * returning a valid path once within thresholds to refine path comes at more
   * compute time but smoother paths.
   * @param max_planning_time Maximum time (in seconds) to wait for a plan, \ref
   * AStarAlgorithm::create_path returns false after this timeout.
   * @param lookup_table_size
   * @param dim_3_size
   */
  void initialize(bool allow_unknown, std::uint32_t max_iterations,
                  std::uint32_t max_on_approach_iterations,
                  double max_planning_time, double lookup_table_size,
                  std::uint32_t dim_3_size);

  /**
   * @brief Create a path from given map, start and goal.
   *
   * @param path Reference to a vector of indicies of generated path
   * @param iterations Reference to number of iterations to create path
   * @param tolerance Reference to tolerance in map nodes
   */
  bool create_path(CoordinateVector &path, std::uint32_t &iterations,
                   const double &tolerance);

  /**
   * @brief Set the collision checker to use
   *
   * @param collision_checker Collision checker to use for checking state
   * validity.
   */
  void set_collision_checker(
      std::shared_ptr<GridCollisionChecker> &collision_checker);

  /**
   * @brief Set the start pose for planning, as a node index.
   *
   * @param mx The node x index of the start pose
   * @param my The node y index of the start pose
   * @param dim_3 The node dim_3 index of the start pose
   * @return true if success
   */
  bool set_start(std::uint32_t mx, std::uint32_t my, std::uint32_t dim_3);

  /**
   * @brief Set the goal pose for planning, as a node index.
   *
   * @param mx The node x index of the goal pose
   * @param my The node y index of the goal pose
   * @param dim_3 The node dim_3 index of the goal pose
   * @return true if success
   */
  bool set_goal(std::uint32_t mx, std::uint32_t my, std::uint32_t dim_3);

  /**
   * @brief Get pointer to start node
   *
   * @return Node pointer to start node
   */
  NodePtr get_start();

  /**
   * @brief Get pointer to goal node
   *
   * @return Node pointer to goal node
   */
  NodePtr get_goal();

  /**
   * @brief Get maximum number of on-approach iterations after within threshold
   *
   * @return Maximum on-approach iterations parameter.
   */
  std::uint32_t get_on_approach_max_iterations();

  /**
   * @brief Get maximum number of iterations to plan
   * @return Reference to Maximum iterations parameter
   */
  std::uint32_t get_max_iterations();

  /**
   * @brief Get tolerance error between nodes.
   * @return Reference to tolerance parameter.
   */
  double &get_tolerance_heuristic();

  /**
   * @brief Get size of graph in x
   *
   * @return Reference to size in x
   */
  std::uint32_t &get_size_x();
  /**
   * @brief Get size of graph in y
   *
   * @return Reference to size in y
   */
  std::uint32_t &get_size_y();
  /**
   * @brief Get number of angle quantization bins (SE2) or Z coordinate (XYZ)
   *
   * @return Reference to number of angle bins / Z dimension
   */
  std::uint32_t &get_size_dim3();

protected:
  /**
   * @brief Get pointer to next goal in open set
   * @return Node pointer reference to next heuristically scored node
   */
  inline NodePtr get_next_node();
  /**
   * @brief Add a node to the open set
   *
   * @param cost The cost to sort into the open set of the node
   * @param node Node pointer reference to add to open set
   */
  inline void add_node(double cost, NodePtr node);
  /**
   * @brief Adds node to graph
   *
   * @param index Node index to add
   * @return
   */
  inline NodePtr add_to_graph(const std::uint32_t &index);
  /**
   * @brief Check if this node is the goal node
   *
   * @param node Node pointer to check if its the goal node
   * @return if node is goal
   */
  inline bool is_goal(NodePtr node);

  /**
   * @brief Get cost of heuristic of node
   *
   * @param node Node pointer to get heuristic for
   * @return Heuristic cost for node
   */
  inline double get_heuristic_cost(const NodePtr &node);
  /**
   * @brief Check if inputs to planner are valid.
   *
   * @return true if inputs are valid.
   */
  inline bool are_inputs_valid();
  /**
   * @brief Clear heuristic queue of nodes to search.
   */
  inline void clear_queue();
  /**
   * @brief Clear graph of nodes searched
   */
  inline void clear_graph();

  /**
   * @brief Clear start
   */
  void clear_start();

  NodeQueue queue_;
  Graph graph_;
  NodePtr start_;
  NodePtr goal_;
  Coordinates goal_coordinates_;
  NodeHeuristicPair best_heuristic_node_;

  double tolerance_;
  bool traverse_unknown_;
  std::uint32_t max_iterations_;
  int max_on_approach_iterations_;
  double max_planning_time_;
  std::uint32_t dim3_size_;

  std::shared_ptr<map::costmap_2d::Costmap2D> costmap_;
  std::shared_ptr<GridCollisionChecker> collision_checker_;
};

}  // namespace smac_planner
}  // namespace planning