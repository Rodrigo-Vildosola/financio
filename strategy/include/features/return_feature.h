#pragma once
#include "feature_base.h"
#include <cmath>

class ReturnFeature : public IFeature {
public:
    explicit ReturnFeature(size_t period) : m_period(period) {}

    std::string name() const override { 
        return "return_" + std::to_string(m_period); 
    }

    size_t lookback() const override { 
        return m_period + 1; 
    }

    double compute(const std::unordered_map<std::string, std::deque<MarketData>>& w, const std::string& symbol) override {
        auto it = w.find(symbol);
        if (it==w.end() || it->second.size()<lookback()) return NAN;
        const auto& win = it->second;
        const auto& cur = win.back();
        const auto& prev = win[win.size()-1-m_period];
        return std::log(cur.close / prev.close);
    }

private:
    size_t m_period;
};

class ReturnFeatureFactory : public IFeatureFactory {
public:
    std::string family() const override { return "return"; }
    std::unique_ptr<IFeature> create(const std::string& spec) override {
        // "return_5" → 5
        size_t pos = spec.find('_');
        size_t p = std::stoul(spec.substr(pos+1));
        return std::make_unique<ReturnFeature>(p);
    }
};
