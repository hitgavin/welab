#include "smac_planner/collision_checker.h"
#include "core/geometry/point.h"
#include "costmap/costmap_2d.h"
#include "costmap/footprint_collision_checker.h"

#include <cmath>
#include <limits>

using namespace map::costmap_2d;

namespace planning {
namespace smac_planner {

GridCollisionChecker::GridCollisionChecker(
    std::shared_ptr<map::costmap_2d::Costmap2D> costmap,
    std::uint32_t num_quantizations)
    : FootprintCollisionChecker(costmap) {
  costmap_ = costmap;
  // Convert number of regular bins into angles
  double bin_size = 2 * M_PI / static_cast<double>(num_quantizations);
  angles_.reserve(num_quantizations);
  for (std::uint32_t i = 0; i != num_quantizations; ++i) {
    angles_.push_back(bin_size * i);
  }
}

void GridCollisionChecker::set_footprint(
    const map::costmap_2d::Footprint &footprint, bool radius,
    double possible_collision_cost) {
  possible_collision_cost_ = possible_collision_cost;
  footprint_is_radius_ = radius;

  // Use radius, no caching required
  if (radius) {
    return;
  }

  // No change, no updates required
  if (footprint == unoriented_footprint_ && !oriented_footprints_.empty()) {
    return;
  }

  oriented_footprints_.reserve(angles_.size());
  double sin_th, cos_th;
  core::geometry::Point new_pt;
  std::uint32_t footprint_size = footprint.size();

  // Precompute the orientation bins for checking to use
  for (std::uint32_t i = 0; i < angles_.size(); ++i) {
    sin_th = std::sin(angles_[i]);
    cos_th = std::cos(angles_[i]);
    Footprint oriented_footprint;
    oriented_footprint.reserve(footprint_size);

    for (std::uint32_t j = 0; j < footprint_size; ++j) {
      new_pt.x_ = footprint[j].x_ * cos_th - footprint[j].y_ * sin_th;
      new_pt.y_ = footprint[j].x_ * sin_th + footprint[j].y_ * cos_th;
      oriented_footprint.push_back(new_pt);
    }

    oriented_footprints_.push_back(oriented_footprint);
  }
  unoriented_footprint_ = footprint;
}

bool GridCollisionChecker::in_collision(double x, double y, double theta,
                                        bool traverse_unknown) {
  std::ignore = x;
  std::ignore = y;
  std::ignore = theta;
  std::ignore = traverse_unknown;
  return true;
}

bool GridCollisionChecker::in_collision(std::uint32_t i,
                                        bool traverse_unknown) {
  std::ignore = i;
  std::ignore = traverse_unknown;
  return true;
}
}  // namespace smac_planner
}  // namespace planning