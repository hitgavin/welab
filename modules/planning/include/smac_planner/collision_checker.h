#pragma once

#include <costmap/costmap_2d.h>
#include <costmap/footprint_collision_checker.h>
#include <cstdint>

namespace planning {
namespace smac_planner {

/**
 * @class GridCollisionChecker
 * @brief A costmap grid collision checker
 */
class GridCollisionChecker : public map::costmap_2d::FootprintCollisionChecker<
                                 std::shared_ptr<map::costmap_2d::Costmap2D>> {
public:
  /**
   * @brief A constructor for GridCollisionChecker
   *
   * @param costmap The costmap to collision check against
   * @param num_quantizations The number of quantizations to precompute
   * footprint
   */
  GridCollisionChecker(std::shared_ptr<map::costmap_2d::Costmap2D> costmap,
                       std::uint32_t num_quantizations);

  /**
   * @brief Set the footprint to use with collision checker
   *
   * @param footprint The footprint to collision check against
   * @param radius Whether or not the footprint is a circle and use radius
   * collision checking
   * @param possible_collision_cost
   */
  void set_footprint(const map::costmap_2d::Footprint &footprint, bool radius,
                     double possible_collision_cost);
  /**
   * @brief Check if in collision with costmap and footprint at pose
   *
   * @param x x coordinate of pose to check against
   * @param y y coordinate of pose to check against
   * @param theta angle bin number of pose to check against (not radians)
   * @param traverse_unknown Whether or not to traverse in unknown space
   * @return if in collision or not.
   */
  bool in_collision(double x, double y, double theta, bool traverse_unknown);
  /**
   * @brief Check if in collision with costmap and footprint at pose
   *
   * @param i Index to search collision status of node
   * @param traverse_unknown Whether or not to traverse in unknown space
   * @return if in collision or not.
   */
  bool in_collision(std::uint32_t i, bool traverse_unknown);

  /**
   * @brief Get the angles of the precomputed footprint orientations
   *
   * @return The ordered vector of angles corresponding to footprints
   */
  const std::vector<double> &get_precomputed_angles() { return angles_; }

protected:
  double possible_collision_cost_{-1.0};
  bool footprint_is_radius_{false};
  map::costmap_2d::Footprint unoriented_footprint_;

  std::vector<double> angles_;
  std::vector<map::costmap_2d::Footprint> oriented_footprints_;
};

}  // namespace smac_planner
}  // namespace planning