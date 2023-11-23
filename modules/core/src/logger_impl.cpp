#include "private/logger_p/logger_impl.h"
#include "core/logger/logger_settings.h"

#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <memory>

namespace core {
namespace logger {

bool LoggerImpl::init() {
  std::vector<std::shared_ptr<spdlog::sinks::sink>> sinks;

  spdlog::init_thread_pool(8192, 1);

  if (settings_.sink_type_ & core::logger::CONSOLE) {
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto level = static_cast<spdlog::level::level_enum>(
        settings_.console_logger_settings_.level_);
    console_sink->set_level(level);
    console_sink->set_pattern("[%^%l%$] %v");
    sinks.push_back(console_sink);
  }
  if (settings_.sink_type_ & core::logger::FILE) {
    auto async_file_sink =
        std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            settings_.file_logger_settings_.file_path_,
            settings_.file_logger_settings_.max_file_size_mb_ * 1024 * 1024,
            settings_.file_logger_settings_.max_files_);
    auto level = static_cast<spdlog::level::level_enum>(
        settings_.file_logger_settings_.level_);
    async_file_sink->set_level(level);
    sinks.push_back(async_file_sink);
  }

  logger_ = std::make_shared<spdlog::async_logger>(
      settings_.logger_name_, sinks.begin(), sinks.end(), spdlog::thread_pool(),
      spdlog::async_overflow_policy::block);

  spdlog::register_logger(logger_);

  init_ = true;

  return true;
}

void LoggerImpl::reset() {
  spdlog::drop_all();
  logger_.reset();
  init_ = false;
}

}  // namespace logger
}  // namespace core