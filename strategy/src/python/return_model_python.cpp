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


void Py_ReturnModel::load(const json& spec) {
    using namespace std;

    cerr << "[Py_ReturnModel] Initializing Python interpreter\n";
    auto& rt = python_runtime::instance();

    string src = spec.at("source").get<string>();
    string cls = spec.at("class").get<string>();
    cerr << "[Py_ReturnModel] Loading class: " << cls << "\n";

    py::dict globals;
    globals["__builtins__"] = py::module_::import("builtins");

    try {
        py::exec(src, globals);
    } catch (const py::error_already_set& e) {
        cerr << "[Py_ReturnModel] Python exec error:\n" << e.what() << "\n";
        throw;
    }

    if (!globals.contains(cls.c_str())) {
        cerr << "[Py_ReturnModel] Class not found in globals\n";
        throw runtime_error("Class " + cls + " not found in source");
    }

    py::object py_cls = globals[cls.c_str()];
    py::object instance;

    if (spec.contains("init")) {
        cerr << "[Py_ReturnModel] Creating instance with kwargs\n";
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
            cerr << "[Py_ReturnModel] Instance creation failed:\n" << e.what() << "\n";
            throw;
        }
    } else {
        cerr << "[Py_ReturnModel] Creating instance without kwargs\n";
        instance = py_cls();
    }

    py::object base_module = py::module_::import("model_base");
    py::object model = base_module.attr("ReturnModelBase");
    if (!py::isinstance(instance, model)) {
        throw std::runtime_error("Python model must subclass ReturnModelBase");
    }

    m_impl->instance = std::move(instance);

    if (!py::hasattr(m_impl->instance, "predict")) {
        cerr << "[Py_ReturnModel] Error: predict() missing\n";
        throw runtime_error("Python model missing predict()");
    }

    cerr << "[Py_ReturnModel] Model loaded successfully\n";
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
