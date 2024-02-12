#include "logger/logger.h"

#include <pybind11/pybind11.h>

PYBIND11_MODULE(core, m) {
  m.doc() = R"(Python bindings for core module)";
  welab_py::bind_logger::init_bind_logger(m);
}