#pragma once

#include "costmap_2d_settings.h"

#include <core/macro/macro.h>

#include <cstdint>
#include <memory>

namespace std {
FORWARD_DECLARE_TYPE(mutex);
}

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
  /**
   * @brief Constructor for a costmap.
   * @param settings The settings of the costmap.
   */
  Costmap2D(const Costmap2DSettings &settings);
  /**
   * @brief Copy constructor for a costmap.
   * @param map The costmap to copy.
   */
  Costmap2D(const Costmap2D &map);
  /**
   * @brief Overloaded assignment operator.
   *
   * @param map The costmap to copy.
   * @return A reference to the costmap after the copy has finished.
   */
  Costmap2D &operator=(const Costmap2D &map);

  /**
   * @brief Default constructor for a costmap.
   */
  Costmap2D();
  /**
   * @brief Destructor for a costmap.
   */
  virtual ~Costmap2D();

  /**
   * @brief Get the cost value at ( \c mx, \c my ) cell.
   *
   * @param mx The x coordinate of the cell.
   * @param my The y coordinate of the cell.
   * @return The cost of the cell.
   * @note It is your responsibility to ensure that the cell you want to
   * access does not cross the boundary, otherwise undefined behavior will
   * occur!
   */
  unsigned char get_cost(std::uint32_t mx, std::uint32_t my);
  /**
   * @brief Set the cost value at ( \c mx, \c my ) cell.
   *
   * @param mx The x coordinate of the cell.
   * @param my The y coordinate of the cell.
   * @param cost The cost value you want to set to the cell.
   * @note Same to \ref Costmap2D::get_cost, It is your responsibility to
   * ensure that the cell you want to access does not cross the boundary.
   */
  void set_cost(std::uint32_t mx, std::uint32_t my, unsigned char cost);

  /**
   * @brief Initializes the costmap.
   * @param size_x The x size of the map in cells.
   * @param size_y The y size of the map in cells.
   */
  virtual void init_maps(std::uint32_t size_x, std::uint32_t size_y);

  /**
   * @brief Reset the costmap.
   */
  virtual void reset_maps();

  /**
   * @brief Delete the costmap.
   */
  virtual void delete_maps();

  /**
   * @brief Given two map coordinates ( \c mx, \c my ), compute the associated
   * index.
   *
   * @param mx The x coordinate of the cell.
   * @param my The y coordinate of the cell.
   * @return The associated index of the map.
   */
  inline std::uint32_t get_index(std::uint32_t mx, std::uint32_t my) {
    return settings_.cells_size_x_ * my + mx;
  }

protected:
  Costmap2DSettings settings_;
  std::shared_ptr<unsigned char[]> costmap_;

private:
  std::mutex *access_;
};

}  // namespace costmap_2d
/** @} */
}  // namespace map
   /** @} */