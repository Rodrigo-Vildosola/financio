#pragma once

#include "feature_extractor.h"
#include "engine.h"
#include "python/execution_model_python.h"
#include "python/risk_model_python.h"
#include "python/return_model_python.h"
#include "python/strategy_python.h"

// class Py_ReturnModel;
// class Py_RiskModel;
// class Py_Strategy;
// class Py_ExecutionModel;

class ResearchEngine : public IEngine {
public:
    ResearchEngine() : m_env(Environment::Research) {}

    void load(const nlohmann::json& spec) override { m_spec = spec; }
    void initialize() override;
    void run() override;
    void finalize() override {}

    Environment environment() const override { return m_env; }

private:
    Environment m_env;
    nlohmann::json m_spec;
    std::unique_ptr<FeatureExtractor> m_extractor;
    std::unique_ptr<Py_Strategy> m_strategy;
    std::unique_ptr<Py_ExecutionModel> m_exec;

    // keep model handles for lifecycle and updates
    std::vector<std::unique_ptr<Py_ReturnModel>> m_return_models;
    std::vector<std::unique_ptr<Py_RiskModel>>   m_risk_models;

    void instantiate_models(const nlohmann::json& strat_init);
};
