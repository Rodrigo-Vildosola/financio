#pragma once
#include "../market_data.h"
#include <deque>
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>

// single feature
class IFeature {
public:
    virtual ~IFeature() = default;
    virtual std::string name() const = 0;
    virtual size_t lookback() const = 0;
    virtual double compute(const std::unordered_map<std::string,std::deque<MarketData>>& windows, const std::string& symbol) = 0;
};

// factory creating features by spec string
class IFeatureFactory {
public:
    virtual ~IFeatureFactory() = default;
    virtual std::string family() const = 0;
    virtual std::unique_ptr<IFeature> create(const std::string& spec) = 0;
};
