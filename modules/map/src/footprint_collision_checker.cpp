#include "costmap/footprint_collision_checker.h"
#include "costmap/costmap_2d.h"

#include <tuple>

namespace map {
namespace costmap_2d {

template <typename CostmapT>
FootprintCollisionChecker<CostmapT>::FootprintCollisionChecker()
    : costmap_(nullptr) {}

template <typename CostmapT>
FootprintCollisionChecker<CostmapT>::FootprintCollisionChecker(CostmapT costmap)
    : costmap_(costmap) {}

template <typename CostmapT>
double FootprintCollisionChecker<CostmapT>::footprint_cost(
    const Footprint &footprint) {
  std::ignore = footprint;
  return 0.0;
}

template <typename CostmapT>
void FootprintCollisionChecker<CostmapT>::set_costmap(CostmapT costmap) {
  costmap_ = costmap;
}

template <typename CostmapT>
CostmapT FootprintCollisionChecker<CostmapT>::get_costmap() {
  return costmap_;
}

template class FootprintCollisionChecker<
    std::shared_ptr<map::costmap_2d::Costmap2D>>;

template class FootprintCollisionChecker<map::costmap_2d::Costmap2D *>;

}  // namespace costmap_2d
}  // namespace map