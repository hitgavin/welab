#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace welab_py {
namespace bind_logger {
void init_bind_logger(py::module& m);
}  // namespace bind_logger
}  // namespace welab_py