#pragma once
#include "feature_base.h"
#include <cmath>
#include <numeric>

class VolatilityFeature : public IFeature {
public:
    explicit VolatilityFeature(size_t period) : m_period(period){}

    std::string name() const override { 
        return "volatility_" + std::to_string(m_period); 
    }
    size_t lookback() const override { 
        return m_period+1; 
    }

    double compute(const std::unordered_map<std::string, std::deque<MarketData>>& w, const std::string& symbol) override {
        auto it = w.find(symbol);
        if (it==w.end() || it->second.size()<lookback()) return NAN;
        const auto& win = it->second;
        std::vector<double> rets;
        for(size_t i=win.size()-m_period-1;i<win.size()-1;++i)
            rets.push_back(std::log(win[i+1].close/win[i].close));
        double mean = std::accumulate(rets.begin(),rets.end(),0.0)/rets.size();
        double var=0;
        for(double r:rets) var+=(r-mean)*(r-mean);
        return std::sqrt(var/(rets.size()-1));
    }

private:
    size_t m_period;
};

class VolatilityFeatureFactory : public IFeatureFactory {
public:
    std::string family() const override { return "volatility"; }
    std::unique_ptr<IFeature> create(const std::string& spec) override {
        size_t pos = spec.find('_');
        size_t p = std::stoul(spec.substr(pos+1));
        return std::make_unique<VolatilityFeature>(p);
    }
};
