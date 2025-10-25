#pragma once
#include "feature_vector.h"
#include <nlohmann/json.hpp>
#include <pybind11/embed.h>
#include <string>

namespace py = pybind11;
using json = nlohmann::json;

namespace pyhelpers {

py::dict to_pydict(const FeatureVector& f);

py::object import_class_from_spec(const json& spec, const std::string& cls);

py::object json_to_pyobject(const json& j);

py::object instantiate_class_with_kwargs(const py::object& py_cls, const json& spec);

void validate_model_instance(
    const py::object& instance,
    const std::string& base_module_name,
    const std::string& base_class_name,
    const std::string& required_method_name
);

} // namespace pyhelpers
