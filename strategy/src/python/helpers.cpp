#include "python/helpers.h"
#include <iostream>

namespace py = pybind11;
using json = nlohmann::json;

namespace pyhelpers {

py::dict to_pydict(const FeatureVector& f) {
    py::dict d;
    py::list xs;
    for (auto v : f.x) xs.append(v);
    d["x"] = xs;

    py::dict named;
    for (auto& kv : f.named) named[py::str(kv.first)] = kv.second;
    d["named"] = named;

    py::dict tags;
    for (auto& kv : f.tags) tags[py::str(kv.first)] = py::str(kv.second);
    d["tags"] = tags;

    d["symbol"] = py::str(f.symbol);
    d["timestamp"] = f.timestamp;
    return d;
}


py::object import_class_from_spec(const json& spec, const std::string& cls) {
    using namespace std;

    if (spec.contains("import")) {
        string module_name = spec.at("import").get<string>();
        cerr << "[PyHelpers] Importing module: " << module_name << "\n";
        py::object mod = py::module_::import(module_name.c_str());
        if (!py::hasattr(mod, cls.c_str()))
            throw runtime_error("Class " + cls + " not found in module " + module_name);
        return mod.attr(cls.c_str());
    }

    if (spec.contains("source")) {
        string src = spec.at("source").get<string>();
        py::dict globals;
        globals["__builtins__"] = py::module_::import("builtins");
        py::exec(src, globals);
        if (!globals.contains(cls.c_str()))
            throw runtime_error("Class " + cls + " not found in source");
        return globals[cls.c_str()];
    }

    throw runtime_error("Model spec must include 'import' or 'source'");
}


py::object instantiate_class_with_kwargs(const py::object& py_cls, const json& spec) {
    if (!spec.contains("init"))
        return py_cls();

    py::dict kwargs;
    for (auto it = spec["init"].begin(); it != spec["init"].end(); ++it) {
        const auto& key = it.key();
        if (it->is_array()) {
            py::list arr;
            for (auto& v : *it) arr.append(v.get<double>());
            kwargs[py::str(key)] = arr;
        } else if (it->is_number_float() || it->is_number_integer()) {
            kwargs[py::str(key)] = it->get<double>();
        } else if (it->is_string()) {
            kwargs[py::str(key)] = py::str(it->get<std::string>());
        }
    }
    return py_cls(**py::kwargs(kwargs));
}

void validate_model_instance(
    const py::object& instance,
    const std::string& base_module_name,
    const std::string& base_class_name,
    const std::string& required_method_name
) {
    py::object base_module = py::module_::import(base_module_name.c_str());
    if (!py::hasattr(base_module, base_class_name.c_str())) {
        throw std::runtime_error("[PyHelpers] Base class " + base_class_name + " not found in module " + base_module_name);
    }

    py::object base_class = base_module.attr(base_class_name.c_str());
    if (!py::isinstance(instance, base_class)) {
        throw std::runtime_error("[PyHelpers] Python model must subclass " + base_class_name);
    }

    if (!py::hasattr(instance, required_method_name.c_str())) {
        throw std::runtime_error("[PyHelpers] Python model missing method '" + required_method_name + "'");
    }
}


} // namespace pyhelpers
