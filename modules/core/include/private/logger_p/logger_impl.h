#pragma once

#include "core/logger/logger_settings.h"

#ifndef SPDLOG_FMT_EXTERNAL
#define SPDLOG_FMT_EXTERNAL
#endif
#include <spdlog/spdlog.h>

#include <unordered_map>

namespace core {
namespace logger {

struct LoggerImpl {
  bool init();
  void reset();

  LoggerSettings settings_;
  bool init_{false};

  std::shared_ptr<spdlog::logger> logger_;
};

}  // namespace logger
}  // namespace core