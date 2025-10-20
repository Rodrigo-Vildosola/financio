#include "python/python_runtime.h"

#include <pybind11/embed.h>

namespace py = pybind11;

python_runtime::python_runtime() : m_guard{} {}
python_runtime::~python_runtime() = default;

void python_runtime::add_sys_path(const std::string& p) {
    // safe to call many times
    py::module_ sys = py::module_::import("sys");
    auto path = sys.attr("path");
    if (!py::bool_(py::module_::import("builtins").attr("any")(
            py::make_tuple(path.contains(p))))) {
        path.attr("append")(p);
    }
}
