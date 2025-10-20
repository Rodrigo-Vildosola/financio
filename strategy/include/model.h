#pragma once

#include <nlohmann/json.hpp>
#include <vector>
#include <unordered_map>
#include <string>

class FeatureVector;

// Predictive model: outputs expected return (mu)
class IReturnModel {
public:
    virtual ~IReturnModel() = default;

    // Initialize model from JSON spec (weights, parameters, etc.)
    virtual void load(const nlohmann::json& spec) = 0;

    // Predict expected return μ given feature vector
    virtual double predict(const FeatureVector& f) = 0;

    // Optional online update step
    virtual void update(const FeatureVector& f, double target) = 0;

    virtual nlohmann::json save_state() const { 
        return {}; 
    }
};

// Risk model: outputs volatility (sigma)
class IRiskModel {
public:
    virtual ~IRiskModel() = default;

    // Initialize risk model from JSON spec
    virtual void load(const nlohmann::json& spec) = 0;

    // Estimate σ (standard deviation or risk metric)
    virtual double estimate(const FeatureVector& f) = 0;

    // Optional update if the risk model adapts
    virtual void update(const FeatureVector& f, double realized_return) = 0;

    virtual nlohmann::json save_state() const { 
        return {}; 
    }
};
