#pragma once

#include "cost_values.h"

#include <cstdint>

namespace map {
namespace costmap_2d {

/**
 * @struct Costmap2DSettings
 * @brief \ref map::costmap_2d::Costmap2D construction settings.
 */
struct Costmap2DSettings {
  /// @brief The x size of the map in cells
  std::uint32_t cells_size_x_{0};
  /// @brief The y size of the map in cells
  std::uint32_t cells_size_y_{0};
  /// @brief The resolution of the map in meters/cell
  double resolution_{0.0};
  /// @brief The x origin of the map
  double origin_x_{0.0};
  /// @brief The y origin of the map
  double origin_y_{0.0};
  /// @brief Default value
  unsigned char default_value_{FREE_SPACE};

  static Costmap2DSettings default_settings() {
    return {.cells_size_x_ = 0,
            .cells_size_y_ = 0,
            .resolution_ = 0.0,
            .origin_x_ = 0.0,
            .origin_y_ = 0.0,
            .default_value_ = FREE_SPACE};
  }
};

}  // namespace costmap_2d
}  // namespace map