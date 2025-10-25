#pragma once

#include "feature_extractor.h"
#include "engine.h"
#include "python/execution_model_python.h"
#include "python/strategy_python.h"

class LiveEngine : public IEngine {
public:
    LiveEngine() : m_env(Environment::Live) {}

    void load(const nlohmann::json& spec) override { 
        m_spec = spec; 
    }

    void initialize() override;
    void run() override;
    void finalize() override;

    Environment environment() const override { 
        return m_env; 
    }

private:
    Environment m_env;
    nlohmann::json m_spec;
    std::unique_ptr<FeatureExtractor> m_extractor;
    std::unique_ptr<Py_Strategy> m_strategy;
    std::unique_ptr<Py_ExecutionModel> m_exec;
};
