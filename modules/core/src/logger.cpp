#include "core/logger/logger.h"

#include "core/logger/logger_settings.h"
#include "private/logger_p/logger_impl.h"

#include <iostream>
#include <memory>

namespace core {
namespace logger {

Logger::~Logger() { impl_->reset(); }

void Logger::trace(const std::string &msg) {
  if (!is_init()) {
    return;
  }
  impl_->logger_->trace(msg);
}

void Logger::debug(const std::string &msg) {
  if (!is_init()) {
    return;
  }
  impl_->logger_->debug(msg);
}

void Logger::info(const std::string &msg) {
  if (!is_init()) {
    return;
  }
  impl_->logger_->info(msg);
}

void Logger::warn(const std::string &msg) {
  if (!is_init()) {
    return;
  }
  std::cout << this << std::endl;
  impl_->logger_->warn(msg);
}

void Logger::error(const std::string &msg) {
  if (!is_init()) {
    return;
  }
  impl_->logger_->error(msg);
}

void Logger::critical(const std::string &msg) {
  if (!is_init()) {
    return;
  }
  impl_->logger_->critical(msg);
}

const LoggerSettings &Logger::logger_settings() const {
  return impl_->settings_;
}

void Logger::flush() { impl_->logger_->flush(); }

void Logger::set_logger_settings(const LoggerSettings &settings) {
  impl_->settings_ = settings;
}

bool Logger::is_init() const { return impl_->init_; }

bool Logger::init() {
  if (is_init()) {
    reset_logger();
  }
  return impl_->init();
}

void Logger::reset_logger() { impl_->reset(); }

Logger::Logger() : impl_(std::make_unique<LoggerImpl>()) {}

}  // namespace logger
}  // namespace core

core::logger::Logger::SharedPtr logger() {
  return core::logger::Logger::instance();
}