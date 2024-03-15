#pragma once

#include <core/geometry/point.h>

#include <vector>

namespace map {
namespace costmap_2d {

typedef std::vector<core::geometry::Point> Footprint;

template <typename CostmapT>
class FootprintCollisionChecker {
public:
  FootprintCollisionChecker();
  explicit FootprintCollisionChecker(CostmapT costmap);

  /**
   * @brief Find the footprint cost in oriented footprint.
   */
  double footprint_cost(const Footprint& footprint);
  /**
   * @brief Set the current costmap to use for collision detection.
   */
  void set_costmap(CostmapT costmap);
  /**
   * @brief Get the current costmap
   */
  CostmapT get_costmap();

protected:
  CostmapT costmap_;
};

}  // namespace costmap_2d
}  // namespace map