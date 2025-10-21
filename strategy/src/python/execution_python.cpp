#include "python/execution_python.h"
#include "python/helpers.h"
#include "python/python_runtime.h"
#include <stdexcept>

namespace py = pybind11;
using json = nlohmann::json;

struct Py_Execution::Impl { 
    py::object instance; 
};

Py_Execution::Py_Execution() : m_impl(std::make_unique<Impl>()) {}
Py_Execution::~Py_Execution() = default;

void Py_Execution::load(const json& spec) {
    auto& rt = python_runtime::instance();
    std::string cls = spec.at("class").get<std::string>();
    py::object py_cls = pyhelpers::import_class_from_spec(spec, cls);
    py::object instance = pyhelpers::instantiate_class_with_kwargs(py_cls, spec);
    pyhelpers::validate_model_instance(instance, "model_base", "ExecutionModelBase", "generate_orders");
    m_impl->instance = std::move(instance);
}

json Py_Execution::generate_orders(double target_weight, const json& context) {
    py::gil_scoped_acquire gil;
    py::object out = m_impl->instance.attr("generate_orders")(target_weight, py::cast(context));
    // Convert py list/dict -> nlohmann::json
    return out.cast<json>();
}
