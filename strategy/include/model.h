#pragma once

#include <nlohmann/json.hpp>
#include <vector>
#include <unordered_map>
#include <string>

class FeatureVector;

class IModel {
public:
    virtual ~IModel() = default;
    virtual void load(const nlohmann::json& spec) = 0;               // artifacts, params
    virtual double predict(const FeatureVector& features) = 0;        // main output (e.g., μ)
    virtual void update(const FeatureVector&, double target) {}       // optional online learning
    virtual nlohmann::json save_state() const { return {}; }
};
