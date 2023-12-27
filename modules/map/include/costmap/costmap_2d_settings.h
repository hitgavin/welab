#pragma once

#include <cstdint>
#include <vector>

namespace map {
namespace costmap_2d {

/**
 * @struct Costmap2DSettings
 * @brief \ref map::costmap_2d::Costmap2D construction settings.
 */
struct Costmap2DSettings {
  /// @brief The x size of the map in cells
  std::uint32_t cells_size_x_;
  /// @brief The y size of the map in cells
  std::uint32_t cells_size_y_;
  /// @brief The resolution of the map in meters/cell
  double resolution_;
  /// @brief The x origin of the map
  double origin_x_;
  /// @brief The y origin of the map
  double origin_y_;
  /// @brief Default value
  unsigned char default_value_;
};

}  // namespace costmap_2d
}  // namespace map