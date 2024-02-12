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

#include "node_basic.h"
#include "smac_planner_types.h"

#include <robin_hood.h>

#include <cstdint>
#include <queue>
#include <utility>

namespace planning {
namespace smac_planner {

template <typename NodeT>
class AStarAlgorithm {
public:
  typedef NodeT *NodePtr;
  typedef std::pair<double, NodeBasic<NodeT>> NodeElement;
  typedef typename NodeT::Coordinates Coordinates;
  typedef typename NodeT::CoordinateVector CoordinateVector;
  typedef robin_hood::unordered_node_map<std::uint32_t, NodeT> Graph;

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
   * @brief Create a path from given map, start and goal.
   *
   * @param path Reference to a vector of indicies of generated path
   * @param num_iterations Reference to number of iterations to create path
   * @param tolerance Reference to tolerance in map nodes
   */
  bool create_path(CoordinateVector &path, std::uint32_t &num_iterations,
                   const double &tolerance);

  /**
   * @brief Set the start pose for planning, as a node index.
   *
   * @param mx The node x index of the start pose
   * @param my The node y index of the start pose
   * @param dim_3 The node dim_3 index of the start pose
   */
  void set_start(const std::uint32_t &mx, const std::uint32_t &my,
                 const std::uint32_t &dim_3);

  /**
   * @brief Set the goal pose for planning, as a node index.
   *
   * @param mx The node x index of the goal pose
   * @param my The node y index of the goal pose
   * @param dim_3 The node dim_3 index of the goal pose
   */
  void set_goal(const std::uint32_t &mx, const std::uint32_t &my,
                const std::uint32_t &dim_3);

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

protected:
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

  NodeQueue queue_;
  Graph graph_;
  NodePtr start_;
  NodePtr goal_;
  NodeHeuristicPair best_heuristic_node_;
  double tolerance_;
};

}  // namespace smac_planner
}  // namespace planning