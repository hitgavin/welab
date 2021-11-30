/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2021, gavin.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of the Willow Garage nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/

#include "console_bridge/console.hpp"

#include <cstdarg>
#include <cstdio>
#include <iostream>
#include <mutex>
#include <string>
#include <utility>

#include "macros/console_colors.hpp"

/// @cond IGNORE

struct DefaultOutputHandler {
  DefaultOutputHandler() {
    output_handler_ = static_cast<console_bridge::OutputHandler *>(&std_output_handler_);
    previous_output_handler_ = output_handler_;
    log_level_ = console_bridge::CONSOLE_BRIDGE_LOG_WARN;
  }

  console_bridge::OutputHandlerSTD std_output_handler_;
  console_bridge::OutputHandler *output_handler_;
  console_bridge::OutputHandler *previous_output_handler_;
  console_bridge::LogLevel log_level_;
  std::mutex lock_;
};

// We use this function because we want to handle static initialization correctly.
// However the first run of this function is not thread safe, due to the use of a static
// variable inside the function. For this reason, we ensure the first call happens during
// static initialization using a proxy class

static DefaultOutputHandler *getDOH() {
  static DefaultOutputHandler DOH;
  return &DOH;
}

#define USE_DOH                         \
  DefaultOutputHandler *doh = getDOH(); \
  std::lock_guard<std::mutex> lock_guard(doh->lock_)

#define MAX_BUFFER_SIZE 1024

/// @endcond

void console_bridge::noOutputHandler() {
  USE_DOH;
  doh->previous_output_handler_ = doh->output_handler_;
  doh->output_handler_ = NULL;
}

void console_bridge::restorePreviousOutputHandler() {
  USE_DOH;
  std::swap(doh->previous_output_handler_, doh->output_handler_);
}

void console_bridge::useOutputHandler(OutputHandler *oh) {
  USE_DOH;
  doh->previous_output_handler_ = doh->output_handler_;
  doh->output_handler_ = oh;
}

console_bridge::OutputHandler *console_bridge::getOutputHandler() { return getDOH()->output_handler_; }

void console_bridge::log(const char *file, int line, LogLevel level, const char *m, ...) {
  USE_DOH;
  if (doh->output_handler_ && level >= doh->log_level_) {
    va_list __ap;
    va_start(__ap, m);
    char buf[MAX_BUFFER_SIZE];
#ifdef _MSC_VER
    vsnprintf_s(buf, sizeof(buf), _TRUNCATE, m, __ap);
#else
    vsnprintf(buf, sizeof(buf), m, __ap);
#endif
    va_end(__ap);
    buf[MAX_BUFFER_SIZE - 1] = '\0';
    doh->output_handler_->log(buf, level, file, line);
  }
}

void console_bridge::setLogLevel(LogLevel level) {
  USE_DOH;
  doh->log_level_ = level;
}

console_bridge::LogLevel console_bridge::getLogLevel() {
  USE_DOH;
  return doh->log_level_;
}

static const char *LogLevelString[4] = {"Debug:   ", "Info:    ", "Warning: ", "Error:   "};

void console_bridge::OutputHandlerSTD::log(const std::string &text, LogLevel level, const char *filename, int line) {
  if (level >= CONSOLE_BRIDGE_LOG_WARN) {
    if (level == CONSOLE_BRIDGE_LOG_WARN) {
      std::cerr << WELAB_CONSOLE_COLOR_YELLOW << LogLevelString[level] << text << std::endl;
      std::cerr << "         at line " << line << " in " << filename << WELAB_CONSOLE_COLOR_RESET << std::endl;
      std::cerr.flush();
    } else if (level == CONSOLE_BRIDGE_LOG_ERROR) {
      std::cerr << WELAB_CONSOLE_COLOR_RED << LogLevelString[level] << text << std::endl;
      std::cerr << "         at line " << line << " in " << filename << WELAB_CONSOLE_COLOR_RESET << std::endl;
      std::cerr.flush();
    }
  } else {
    std::cout << LogLevelString[level] << text << std::endl;
    std::cout.flush();
  }
}

/// @cond

namespace console_bridge {
namespace internal {

struct OutputHandlerFilePrivate {
  FILE *file;
};

}  // namespace internal

}  // namespace console_bridge

/// @endcond

console_bridge::OutputHandlerFile::OutputHandlerFile(const char *filename)
    : OutputHandler(), d_ptr(std::make_unique<internal::OutputHandlerFilePrivate>()) {
#ifdef _MSC_VER
  errno_t err = fopen_s(&d_ptr->file, filename, "a");
  if (err != 0 || !d_ptr->file)
#else
  d_ptr->file = fopen(filename, "a");
  if (!d_ptr->file)
#endif
    std::cerr << "Unable to open log file: '" << filename << "'" << std::endl;
}

console_bridge::OutputHandlerFile::~OutputHandlerFile() {
  if (d_ptr->file) {
    if (fclose(d_ptr->file) != 0) {
      std::cerr << "Error closing log file" << std::endl;
    }
  }
}

void console_bridge::OutputHandlerFile::log(const std::string &text, LogLevel level, const char *filename, int line) {
  if (d_ptr->file) {
    fprintf(d_ptr->file, "%s%s\n", LogLevelString[level], text.c_str());
    if (level >= CONSOLE_BRIDGE_LOG_WARN) {
      fprintf(d_ptr->file, "         at line %d in %s\n", line, filename);
      fflush(d_ptr->file);
    }
  }
}