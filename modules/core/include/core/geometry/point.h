#pragma once

/**
 * @defgroup geometry_group geometry module
 * @ingroup core_group
 * @brief The geometry namespace group for geometry basic type include point,
 * line, quaternion and so on.
 */

namespace core {
/**
 * @ingroup geometry_group
 * @brief The geometry namespace
 * @{
 */
namespace geometry {

struct Point {
  Point() : x_(0.0), y_(0.0), z_(0.0) {}

  double x_;  ///< x coordinate
  double y_;  ///< y coordinate
  double z_;  ///< z coordinate
};

}  // namespace geometry
/** @} */
}  // namespace core