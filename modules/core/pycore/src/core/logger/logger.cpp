#include "logger.h"

#include <core/logger/logger.h>
#include <core/logger/logger_settings.h>

#include <pybind11/detail/common.h>
#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
#include <memory>

using namespace core::logger;

namespace welab_py {
namespace bind_logger {
void init_bind_logger(py::module &m) {
  py::module logger = m.def_submodule("logger");

  py::enum_<SinkType>(logger, "SinkType")
      .value("NONE", SinkType::NONE)
      .value("CONSOLE", SinkType::CONSOLE)
      .value("FILE", SinkType::FILE)
      .export_values();

  py::enum_<Level>(logger, "Level")
      .value("TRACE", Level::TRACE)
      .value("DEBUG", Level::DEBUG)
      .value("INFO", Level::INFO)
      .value("WARN", Level::WARN)
      .value("ERR", Level::ERR)
      .value("CRITICAL", Level::CRITICAL)
      .value("OFF", Level::OFF)
      .export_values();

  py::class_<FileLoggerSettings>(logger, "FileLoggerSettings")
      .def(py::init<>())
      .def_readwrite("level", &FileLoggerSettings::level_)
      .def_readwrite("file_path", &FileLoggerSettings::file_path_)
      .def_readwrite("max_file_size_mb", &FileLoggerSettings::max_file_size_mb_)
      .def_readwrite("max_files", &FileLoggerSettings::max_files_);

  py::class_<ConsoleLoggerSettings>(logger, "ConsoleLoggerSettings")
      .def(py::init<>())
      .def_readwrite("level", &ConsoleLoggerSettings::level_);

  py::class_<LoggerSettings>(logger, "LoggerSettings")
      .def(py::init<>())
      .def_readwrite("sink_type", &LoggerSettings::sink_type_)
      .def_readwrite("logger_name", &LoggerSettings::logger_name_)
      .def_readwrite("file_logger_settings",
                     &LoggerSettings::file_logger_settings_)
      .def_readwrite("console_logger_settings",
                     &LoggerSettings::console_logger_settings_);

  py::class_<Logger, std::unique_ptr<Logger, py::nodelete>>(logger, "Logger")
      .def(py::init([]() {
        return std::unique_ptr<Logger, py::nodelete>(Logger::instance().get());
      }))
      .def("trace",
           static_cast<void (Logger::*)(const std::string &)>(&Logger::trace))
      .def("debug",
           static_cast<void (Logger::*)(const std::string &)>(&Logger::debug))
      .def("info",
           static_cast<void (Logger::*)(const std::string &)>(&Logger::info))
      .def("warn",
           static_cast<void (Logger::*)(const std::string &)>(&Logger::warn))
      .def("error",
           static_cast<void (Logger::*)(const std::string &)>(&Logger::error))
      .def("critical", static_cast<void (Logger::*)(const std::string &)>(
                           &Logger::critical))
      .def("flush", &Logger::flush)
      .def("logger_settings", &Logger::logger_settings)
      .def("set_logger_settings", &Logger::set_logger_settings)
      .def("init", &Logger::init)
      .def("is_init", &Logger::is_init)
      .def("reset_logger", &Logger::reset_logger);
}
}  // namespace bind_logger
}  // namespace welab_py