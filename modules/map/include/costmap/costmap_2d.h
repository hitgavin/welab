#pragma once

#include "costmap_2d_settings.h"

/**
 * @defgroup map_group map module
 * @brief The main namespace group for map module.
 *
 * This group contains all kinds of maps such as costmap, octomap and so on.
 */

/**
 * @defgroup costmap_2d_group costmap_2d submodule
 * @ingroup map_group
 * @brief The costmap_2d namespace group for costmap_2d part of map module.
 *
 * This group contains all things that is needed by the costmap_2d.
 */

/**
 * @ingroup map_group
 * @brief The map namespace.
 * @{
 */
namespace map {
/**
 * @ingroup costmap_2d
 * @brief The costmap_2d namespace
 * @{
 */
namespace costmap_2d {

class Costmap2D {
  friend class Costmap2DTester;  // Needed by gtest

public:
  Costmap2D(const Costmap2DSettings &settings);

protected:
  Costmap2DSettings settings_;
  unsigned char *costmap_;
};

}  // namespace costmap_2d
/** @} */
}  // namespace map
   /** @} */