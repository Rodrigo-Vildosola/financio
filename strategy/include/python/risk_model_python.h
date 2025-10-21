#pragma once
#include "model.h"

#include <pybind11/embed.h>


class Py_RiskModel : public IRiskModel {
public:
    Py_RiskModel();
    ~Py_RiskModel() override;

    void load(const nlohmann::json& spec) override;
    double estimate(const FeatureVector& f) override;
    void update(const FeatureVector& f, double realized_return) override;

    void set_environment(Environment e) override;

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};
