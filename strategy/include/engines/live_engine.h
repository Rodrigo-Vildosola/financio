#pragma once

#include "engine.h"

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
    std::unique_ptr<IStrategy> m_strategy;
};
