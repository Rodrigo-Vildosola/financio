#include "feature_vector.h"

#include "python/risk_model_python.h"
#include "python/python_runtime.h"
#include "python/helpers.h"

#include <iostream>
#include <nlohmann/json.hpp>
#include <pybind11/embed.h>
#include <stdexcept>

namespace py = pybind11;
using json = nlohmann::json;

struct Py_RiskModel::Impl {
    py::object instance;
};

Py_RiskModel::Py_RiskModel() : m_impl(std::make_unique<Impl>()) {}
Py_RiskModel::~Py_RiskModel() = default;

void Py_RiskModel::load(const json& spec) {
    using namespace std;
    cerr << "[Py_RiskModel] Initializing Python interpreter\n";
    auto& rt = python_runtime::instance();

    string src = spec.at("source").get<string>();
    string cls = spec.at("class").get<string>();
    cerr << "[Py_RiskModel] Loading class: " << cls << "\n";

    py::dict globals;
    globals["__builtins__"] = py::module_::import("builtins");

    try {
        py::exec(src, globals);
    } catch (const py::error_already_set& e) {
        cerr << "[Py_RiskModel] Python exec error:\n" << e.what() << "\n";
        throw;
    }

    if (!globals.contains(cls.c_str())) {
        cerr << "[Py_RiskModel] Class not found in globals\n";
        throw runtime_error("Class " + cls + " not found in source");
    }

    py::object py_cls = globals[cls.c_str()];
    py::object instance;

    if (spec.contains("init")) {
        cerr << "[Py_RiskModel] Creating instance with kwargs\n";
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
            cerr << "[Py_RiskModel] Instance creation failed:\n" << e.what() << "\n";
            throw;
        }
    } else {
        cerr << "[Py_RiskModel] Creating instance without kwargs\n";
        instance = py_cls();
    }

    m_impl->instance = std::move(instance);

    if (!py::hasattr(m_impl->instance, "estimate")) {
        cerr << "[Py_RiskModel] Error: estimate() missing\n";
        throw runtime_error("Python risk model missing estimate()");
    }

    cerr << "[Py_RiskModel] Risk model loaded successfully\n";
}

double Py_RiskModel::estimate(const FeatureVector& f) {
    py::gil_scoped_acquire gil;
    auto d = to_pydict(f);
    py::object out = m_impl->instance.attr("estimate")(d);
    return out.cast<double>();
}

void Py_RiskModel::update(const FeatureVector& f, double realized_return) {
    if (!py::hasattr(m_impl->instance, "update")) return;
    py::gil_scoped_acquire gil;
    auto d = to_pydict(f);
    m_impl->instance.attr("update")(d, realized_return);
}
