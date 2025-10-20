#include "python/model_python.h"
#include "feature_vector.h"
#include "python/python_runtime.h"
#include <nlohmann/json.hpp>
#include <pybind11/embed.h>
#include <stdexcept>

namespace py = pybind11;
using json = nlohmann::json;

static py::dict to_pydict(const FeatureVector& f) {
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

struct ModelPython::Impl {
    py::object instance;
};

ModelPython::ModelPython() : m_impl(std::make_unique<Impl>()) {}
ModelPython::~ModelPython() = default;


void ModelPython::load(const json& spec) {
    // ensure interpreter
    auto& rt = python_runtime::instance();


    std::string src = spec.at("source").get<std::string>();
    std::string cls = spec.at("class").get<std::string>();

    py::dict globals = py::dict(py::module_::import("builtins"));
    py::exec(src, globals);

    if (!globals.contains(cls.c_str()))
        throw std::runtime_error("Class " + cls + " not found in source");

    py::object py_cls = globals[cls.c_str()];
    py::object instance;

    if (spec.contains("init")) {
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

        // --- FIXED: explicitly pass as kwargs ---
        instance = py_cls(**py::kwargs(kwargs));
    } else {
        instance = py_cls();
    }

    m_impl->instance = std::move(instance);

    // basic interface check
    if (!py::hasattr(m_impl->instance, "predict"))
        throw std::runtime_error("Python model missing predict()");
}


double ModelPython::predict(const FeatureVector& f) {
    py::gil_scoped_acquire gil;
    auto d = to_pydict(f);
    py::object out = m_impl->instance.attr("predict")(d);
    return out.cast<double>();
}

void ModelPython::update(const FeatureVector& f, double target) {
    if (!py::hasattr(m_impl->instance, "update")) return;
    py::gil_scoped_acquire gil;
    auto d = to_pydict(f);
    m_impl->instance.attr("update")(d, target);
}
