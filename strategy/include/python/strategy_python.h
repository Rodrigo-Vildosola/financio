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
    void load(const nlohmann::json& spec) override;

    void load_with_kwargs(const nlohmann::json& spec, const pybind11::dict& kwargs);


    // Returns target weight (double)
    double on_bar(const FeatureVector& f) override;

    // Pass realized return to submodels via strategy.update()
    void update(const FeatureVector& f, double realized_return) override;

    void set_environment(Environment e) override;

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};
