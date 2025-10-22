#include "feature_vector.h"

#include "python/return_model_python.h"
#include "python/python_runtime.h"
#include "python/helpers.h"

#include <iostream>
#include <nlohmann/json.hpp>
#include <pybind11/embed.h>
#include <stdexcept>

namespace py = pybind11;
using json = nlohmann::json;


struct Py_ReturnModel::Impl {
    py::object instance;
};

Py_ReturnModel::Py_ReturnModel() : m_impl(std::make_unique<Impl>()) {}
Py_ReturnModel::~Py_ReturnModel() = default;

const py::object& Py_ReturnModel::instance() const { 
    return m_impl->instance; 
}



void Py_ReturnModel::load(const json& spec) {
    using namespace std;

    cerr << "[Py_ReturnModel] Initializing Python interpreter\n";
    auto& rt = python_runtime::instance();

    string cls = spec.at("class").get<string>();
    py::object py_cls = pyhelpers::import_class_from_spec(spec, cls);

    py::object instance = pyhelpers::instantiate_class_with_kwargs(py_cls, spec);


    pyhelpers::validate_model_instance(
        instance, 
        "model_base", 
        "ReturnModelBase", 
        "predict"
    );

    m_impl->instance = std::move(instance);
    cerr << "[Py_ReturnModel] Model loaded successfully\n";
}



double Py_ReturnModel::predict(const FeatureVector& f) {
    py::gil_scoped_acquire gil;
    auto d = pyhelpers::to_pydict(f);
    py::object out = m_impl->instance.attr("predict")(d);
    return out.cast<double>();
}

void Py_ReturnModel::update(const FeatureVector& f, double target) {
    if (!py::hasattr(m_impl->instance, "update")) return;
    py::gil_scoped_acquire gil;
    auto d = pyhelpers::to_pydict(f);
    m_impl->instance.attr("update")(d, target);
}

void Py_ReturnModel::set_environment(Environment e) {
    if (!m_impl || !py::hasattr(m_impl->instance, "set_environment"))
        return;
    py::gil_scoped_acquire gil;
    std::string env_str = to_string(e);
    m_impl->instance.attr("set_environment")(env_str);
}
