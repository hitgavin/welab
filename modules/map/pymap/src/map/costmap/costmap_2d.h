#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace welab_py {
namespace bind_costmap {

void init_bind_costmap(py::module& m);

}  // namespace bind_costmap
}  // namespace welab_py