#pragma once

#include "python/strategy_python.h"
#include "feature_extractor.h"
#include "engine.h"

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
    std::unique_ptr<Py_Strategy> m_strategy;
    std::unique_ptr<FeatureExtractor> m_extractor;

};
