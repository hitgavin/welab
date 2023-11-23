#pragma once

#include "logger.h"
#include "private/logger_p/logger_impl.h"

namespace core {
namespace logger {
template <typename... Args>
void Logger::trace(const std::string &fmt, Args &&... args) {
  if (!is_init()) {
    return;
  }
  impl_->logger_->trace(fmt, args...);
}
template <typename... Args>
void Logger::debug(const std::string &fmt, Args &&... args) {
  if (!is_init()) {
    return;
  }
  impl_->logger_->debug(fmt, args...);
}
template <typename... Args>
void Logger::info(const std::string &fmt, Args &&... args) {
  if (!is_init()) {
    return;
  }
  impl_->logger_->info(fmt, args...);
}
template <typename... Args>
void Logger::warn(const std::string &fmt, Args &&... args) {
  if (!is_init()) {
    return;
  }
  impl_->logger_->warn(fmt, args...);
}
template <typename... Args>
void Logger::error(const std::string &fmt, Args &&... args) {
  if (!is_init()) {
    return;
  }
  impl_->logger_->error(fmt, args...);
}
template <typename... Args>
void Logger::critical(const std::string &fmt, Args &&... args) {
  if (!is_init()) {
    return;
  }
  impl_->logger_->critical(fmt, args...);
}
}  // namespace logger
}  // namespace core
