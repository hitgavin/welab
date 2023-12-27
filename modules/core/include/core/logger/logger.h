#pragma once

#include "core/macro/macro.h"
#include "core/mode/singleton.h"

#include <memory>
#include <mutex>

/**
 * @defgroup core_group core module
 * @brief The main namespace group for core module.
 *
 * This group contains all basic tools that is needed for other modules.
 * Architecture of the cpp part is defined.
 */

/**
 * @defgroup logger_group logger submodule
 * @ingroup core_group
 * @brief The logger namespace group for logger part of core module.
 *
 * This group support a full function logger for user to write all kinds of
 * information to files, console, etc.
 */

/**
 * @ingroup core_group
 * @brief The core namespace.
 * @{
 */
namespace core {
/**
 * @ingroup logger_group
 * @brief The logger namespace.
 * @{
 */
namespace logger {

FORWARD_DECLARE_TYPE(LoggerImpl)
FORWARD_DECLARE_TYPE(LoggerSettings)

/**
 * @class Logger
 * @brief A full function logger class.
 *
 * This is a singleton class used to write some log information to console and
 * file (based on settings to the logger)asynchronous.
 *
 * @note Initialize is needed before everything work!
 */
class Logger : public core::mode::Singleton<Logger> {
  friend core::mode::Singleton<Logger>;

public:
  ~Logger();
  void trace(const std::string &msg);
  void debug(const std::string &msg);
  void info(const std::string &msg);
  void warn(const std::string &msg);
  void error(const std::string &msg);
  void critical(const std::string &msg);

  template <typename... Args>
  void trace(const std::string &fmt, Args &&... args);
  template <typename... Args>
  void debug(const std::string &fmt, Args &&... args);
  template <typename... Args>
  void info(const std::string &fmt, Args &&... args);
  template <typename... Args>
  void warn(const std::string &fmt, Args &&... args);
  template <typename... Args>
  void error(const std::string &fmt, Args &&... args);
  template <typename... Args>
  void critical(const std::string &fmt, Args &&... args);

  /** @brief Immediately flushes the stream buffer.
   *
   * This function is used to clear the output buffer and force all
   * pending output to be written to the underlying object as soon as
   * possible.
   */
  void flush();

  /**
   * @brief Get current logger settings.
   * @return Current logger settings, a default logger settings will be
   * returned before you do any configuration.
   */
  const LoggerSettings &logger_settings() const;
  /**
   * @brief Set logger settings.
   *
   * You should first call \ref core::logger::Logger::logger_settings to get
   * logger settings. Then, config the settings. Finally, call this member
   * function to set the logger settings.
   *
   * @param[in] settings This is the settings to the logger.
   */
  void set_logger_settings(const LoggerSettings &settings);
  /**
   * @brief Test if the logger is initialized or not.
   * @return true if the logger is initialized, false otherwise.
   */
  bool is_init() const;
  /**
   * @brief Initialize the logger.
   * @note Only after calling this member function, you can write log
   * correctly. If you call this member function more than once, the logger
   * will first reset the logger and reinitialize it.
   */
  bool init();
  /**
   * @brief Reset the logger.
   *
   * @note This member function will reset the logger to the default, you need
   * to reinitialize the logger before you can write log correctly.
   */
  void reset_logger();

  DECLARE_POINTER_TYPE(core::logger::Logger)

private:
  std::unique_ptr<LoggerImpl> impl_;

private:
  /// @brief Constructor of the Logger class.
  Logger();
};

}  // namespace logger
/** @} */
}  // namespace core
/** @} */

/**
 * @brief A helper function used to get logger singleton object
 * @return logger singleton object pointer.
 */
core::logger::Logger::SharedPtr logger();

#include "logger-inl.h"