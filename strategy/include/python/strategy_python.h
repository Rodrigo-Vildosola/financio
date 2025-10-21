#pragma once
#include "model.h"
#include "feature_vector.h"
#include <nlohmann/json.hpp>
#include <pybind11/embed.h>
#include <memory>

class Py_Strategy : IStrategy {
public:
    Py_Strategy();
    ~Py_Strategy();

    // spec: {"import","class","init":{...}}
    void load(const nlohmann::json& spec);

    // Returns target weight (double)
    double on_bar(const FeatureVector& f);

    // Pass realized return to submodels via strategy.update()
    void update(const FeatureVector& f, double realized_return);

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};
