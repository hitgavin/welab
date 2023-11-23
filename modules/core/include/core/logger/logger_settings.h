#pragma once

#include <cstdint>
#include <string>

namespace core {
namespace logger {

/**
 * @enum SinkType
 * @brief The logger support different sink type. CONSOLE and FILE are
 * currently supported.
 */
enum SinkType {
  NONE = 0,
  CONSOLE = 1,
  FILE = 2,
};

/**
 * @enum Level
 * @brief The log level.
 */
enum Level {
  TRACE,
  DEBUG,
  INFO,
  WARN,
  ERR,
  CRITICAL,
  OFF,
};

/**
 * @struct FileLoggerSettings
 * @brief If you want to log to some file, configure member variable in
 * \ref core::logger::LoggerSettings struct.
 */
struct FileLoggerSettings {
  /// @brief Logger level.
  Level level_{INFO};
  /// @brief Logger file full path
  std::string file_path_;
  /// @brief Max logger file size, unit (MB)
  std::uint32_t max_file_size_mb_;
  /// @brief Max logger file count.
  std::uint32_t max_files_;
};

/**
 * @struct ConsoleLoggerSettings
 * @brief If you want to log to console, configure member variable in
 * \ref core::logger::LoggerSettings struct.
 */
struct ConsoleLoggerSettings {
  /// @brief Logger level.
  Level level_{INFO};
};

/**
 * @struct LoggerSettings
 * @brief Settings of the \ref core::logger::Logger object
 */
struct LoggerSettings {
  /// @brief Composition of SinkType, such as CONSOLE|FILE
  std::uint32_t sink_type_{CONSOLE};
  /// @brief Logger name.
  std::string logger_name_{"logger"};
  /// @brief File logger settings
  FileLoggerSettings file_logger_settings_;
  /// @brief Console logger settings
  ConsoleLoggerSettings console_logger_settings_;
};

}  // namespace logger
}  // namespace core
