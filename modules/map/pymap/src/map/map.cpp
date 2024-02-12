#include "costmap/costmap_2d.h"

#include <pybind11/pybind11.h>

PYBIND11_MODULE(map, m) {
  m.doc() = R"(Python bindings for map module)";
  welab_py::bind_costmap::init_bind_costmap(m);
}