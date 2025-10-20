#include "python/model_python.h"
#include "feature_vector.h"
#include "python/python_runtime.h"
#include <iostream>
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

struct Py_ReturnModel::Impl {
    py::object instance;
};

Py_ReturnModel::Py_ReturnModel() : m_impl(std::make_unique<Impl>()) {}
Py_ReturnModel::~Py_ReturnModel() = default;


void Py_ReturnModel::load(const json& spec) {
    using namespace std;
    cerr << "[ModelPython] Initializing Python interpreter\n";
    auto& rt = python_runtime::instance();

    string src = spec.at("source").get<string>();
    string cls = spec.at("class").get<string>();
    cerr << "[ModelPython] Loading class: " << cls << "\n";

    py::dict globals;
    globals["__builtins__"] = py::module_::import("builtins");

    try {
        py::exec(src, globals);
    } catch (const py::error_already_set& e) {
        cerr << "[ModelPython] Python exec error:\n" << e.what() << "\n";
        throw;
    }

    if (!globals.contains(cls.c_str())) {
        cerr << "[ModelPython] Class not found in globals\n";
        throw runtime_error("Class " + cls + " not found in source");
    }

    py::object py_cls = globals[cls.c_str()];
    py::object instance;

    if (spec.contains("init")) {
        cerr << "[ModelPython] Creating instance with kwargs\n";
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
                kwargs[py::str(key)] = py::str(it->get<string>());
            }
        }

        try {
            instance = py_cls(**py::kwargs(kwargs));
        } catch (const py::error_already_set& e) {
            cerr << "[ModelPython] Instance creation failed:\n" << e.what() << "\n";
            throw;
        }
    } else {
        cerr << "[ModelPython] Creating instance without kwargs\n";
        instance = py_cls();
    }

    m_impl->instance = std::move(instance);

    if (!py::hasattr(m_impl->instance, "predict")) {
        cerr << "[ModelPython] Error: predict() missing\n";
        throw runtime_error("Python model missing predict()");
    }

    cerr << "[ModelPython] Model loaded successfully\n";
}



double Py_ReturnModel::predict(const FeatureVector& f) {
    py::gil_scoped_acquire gil;
    auto d = to_pydict(f);
    py::object out = m_impl->instance.attr("predict")(d);
    return out.cast<double>();
}

void Py_ReturnModel::update(const FeatureVector& f, double target) {
    if (!py::hasattr(m_impl->instance, "update")) return;
    py::gil_scoped_acquire gil;
    auto d = to_pydict(f);
    m_impl->instance.attr("update")(d, target);
}
