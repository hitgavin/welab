#include "logger/logger.h"

#include <pybind11/pybind11.h>

int add(int a, int b) { return a + b; }
PYBIND11_MODULE(core, m) {
  m.doc() = R"(Python bindings for core module)";
  welab_py::bind_logger::init_bind_logger(m);
}