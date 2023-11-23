#pragma once

#include "core/macro/macro.h"

#include <memory>
#include <mutex>

/**
 * @defgroup mode_group design pattern submodule
 * @ingroup core_group
 * @brief The mode namespace group for design pattern related.
 *
 * This group support some basic design pattern implementation.
 */

namespace core {
/**
 * @ingroup mode_group
 * @brief The design pattern namespace.
 * @{
 */
namespace mode {

/**
 * @class Singleton
 * @brief A singleton implementation class.
 *
 * This is a singleton class used to create a singleton instance of a class.
 * Derive from this class to make a class a singleton.
 *
 * @tparam T The class type that need to be a singleton.
 */
template <typename T>
class Singleton {
public:
  /**
   * @brief Get the singleton instance.
   *
   * @return std::shared The singleton instance.
   */
  static std::shared_ptr<T> instance() {
    static std::once_flag flag;
    static std::shared_ptr<T> inst;
    std::call_once(flag, [] { inst.reset(new T()); });

    return inst;
  }

protected:
  Singleton() = default;
  ~Singleton() = default;

private:
  DISABLE_COPY(Singleton<T>);
};

}  // namespace mode
/** @} */
}  // namespace core