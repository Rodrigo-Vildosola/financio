#pragma once
#include "model.h"

#include <pybind11/embed.h>

class ModelPython : public IModel {
public:
    ModelPython();
    ~ModelPython() override;

    void load(const nlohmann::json& spec) override;
    double predict(const FeatureVector& f) override;
    void update(const FeatureVector& f, double target) override;

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};
