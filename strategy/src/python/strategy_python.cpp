#include "python/strategy_python.h"
#include "python/helpers.h"
#include "python/python_runtime.h"
#include <stdexcept>

namespace py = pybind11;
using json = nlohmann::json;

struct Py_Strategy::Impl { 
    py::object instance; 
};

Py_Strategy::Py_Strategy() : m_impl(std::make_unique<Impl>()) {}
Py_Strategy::~Py_Strategy() = default;

void Py_Strategy::load(const json& spec) {
    auto& rt = python_runtime::instance();
    std::string cls = spec.at("class").get<std::string>();
    py::object py_cls = pyhelpers::import_class_from_spec(spec, cls);
    py::object instance = pyhelpers::instantiate_class_with_kwargs(py_cls, spec);
    // Validate base
    pyhelpers::validate_model_instance(instance, "model_base", "StrategyBase", "on_bar");
    m_impl->instance = std::move(instance);
}

double Py_Strategy::on_bar(const FeatureVector& f) {
    py::gil_scoped_acquire gil;
    auto d = pyhelpers::to_pydict(f);
    py::object out = m_impl->instance.attr("on_bar")(d);
    return out.cast<double>();
}

void Py_Strategy::update(const FeatureVector& f, double realized_return) {
    if (!py::hasattr(m_impl->instance, "update")) return;
    py::gil_scoped_acquire gil;
    auto d = pyhelpers::to_pydict(f);
    m_impl->instance.attr("update")(d, realized_return);
}
