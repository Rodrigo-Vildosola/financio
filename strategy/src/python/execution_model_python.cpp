#include "python/execution_model_python.h"
#include "environment.h"
#include "python/helpers.h"
#include "python/python_runtime.h"
#include <stdexcept>

namespace py = pybind11;
using json = nlohmann::json;

struct Py_ExecutionModel::Impl { 
    py::object instance; 
};

Py_ExecutionModel::Py_ExecutionModel() : m_impl(std::make_unique<Impl>()) {}
Py_ExecutionModel::~Py_ExecutionModel() = default;

void Py_ExecutionModel::load(const json& spec) {
    auto& rt = python_runtime::instance();
    std::string cls = spec.at("class").get<std::string>();
    py::object py_cls = pyhelpers::import_class_from_spec(spec, cls);
    py::object instance = pyhelpers::instantiate_class_with_kwargs(py_cls, spec);

    pyhelpers::validate_model_instance(
        instance, 
        "model_base", 
        "ExecutionModelBase", 
        "generate_orders"
    );

    m_impl->instance = std::move(instance);
}

nlohmann::json Py_ExecutionModel::generate_orders(double target_weight, const Context& ctx) {
    py::gil_scoped_acquire gil;
    nlohmann::json ctx_json = to_json(ctx);
    py::object out = m_impl->instance.attr("generate_orders")(target_weight, py::cast(ctx_json));
    return out.cast<nlohmann::json>();
}

void Py_ExecutionModel::set_environment(Environment e) {
    if (!m_impl || !py::hasattr(m_impl->instance, "set_environment"))
        return;
    py::gil_scoped_acquire gil;
    std::string env_str = to_string(e);
    m_impl->instance.attr("set_environment")(env_str);
}
