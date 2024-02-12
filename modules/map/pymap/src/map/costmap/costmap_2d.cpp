#include "costmap_2d.h"

#include <costmap/cost_values.h>
#include <costmap/costmap_2d.h>
#include <costmap/costmap_2d_settings.h>
#include <pybind11/pybind11.h>

using namespace map::costmap_2d;

namespace welab_py {
namespace bind_costmap {

void init_bind_costmap(py::module& m) {
  py::module costmap = m.def_submodule("costmap");

  costmap.attr("NO_INFORMATION") = map::costmap_2d::NO_INFORMATION;
  costmap.attr("LETHAL_OBSTACLE") = map::costmap_2d::LETHAL_OBSTACLE;
  costmap.attr("INSCRIBED_INFLATED_OBSTACLE") =
      map::costmap_2d::INSCRIBED_INFLATED_OBSTACLE;
  costmap.attr("MAX_NON_OBSTACLE") = map::costmap_2d::MAX_NON_OBSTACLE;
  costmap.attr("FREE_SPACE") = map::costmap_2d::FREE_SPACE;

  py::class_<Costmap2DSettings>(costmap, "Costmap2DSettings")
      .def(py::init<>())
      .def_readwrite("cells_size_x", &Costmap2DSettings::cells_size_x_)
      .def_readwrite("cells_size_y", &Costmap2DSettings::cells_size_y_)
      .def_readwrite("resolution", &Costmap2DSettings::resolution_)
      .def_readwrite("origin_x", &Costmap2DSettings::origin_x_)
      .def_readwrite("origin_y", &Costmap2DSettings::origin_y_)
      .def_readwrite("default_value", &Costmap2DSettings::default_value_)
      .def_static("default_settings", &Costmap2DSettings::default_settings);

  py::class_<Costmap2D>(costmap, "Costmap2D")
      .def(py::init<const Costmap2DSettings&>())
      .def(py::init<const Costmap2D&>())
      .def(py::init<>())
      .def("get_cost", &Costmap2D::get_cost)
      .def("set_cost", &Costmap2D::set_cost)
      .def("init_maps", &Costmap2D::init_maps)
      .def("reset_maps", &Costmap2D::reset_maps)
      .def("delete_maps", &Costmap2D::delete_maps)
      .def("get_index", &Costmap2D::get_index);
}

}  // namespace bind_costmap
}  // namespace welab_py