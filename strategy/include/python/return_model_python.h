#pragma once
#include "model.h"

#include <pybind11/embed.h>

class Py_ReturnModel : public IReturnModel {
public:
    Py_ReturnModel();
    ~Py_ReturnModel() override;

    void load(const nlohmann::json& spec) override;
    double predict(const FeatureVector& f) override;
    void update(const FeatureVector& f, double target) override;

    void set_environment(Environment e) override;

    const pybind11::object& instance() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

