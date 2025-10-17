#pragma once
#include "feature_base.h"
#include <cmath>
#include <sstream>

class RelativeMomentumFeature : public IFeature {
public:
    RelativeMomentumFeature(std::string ref,size_t p) : m_ref_symbol(std::move(ref)), m_period(p){}

    std::string name() const override {
        std::ostringstream ss; ss<<"rel_momentum_"<<m_ref_symbol<<"_"<<m_period;
        return ss.str();
    }
    size_t lookback() const override { 
        return m_period+1; 
    }

    double compute(const std::unordered_map<std::string, std::deque<MarketData>>& w, const std::string& symbol) override {
        auto it_t=w.find(symbol);
        auto it_r=w.find(m_ref_symbol);
        if(it_t==w.end()||it_r==w.end()) return NAN;
        const auto& wt=it_t->second; const auto& wr=it_r->second;
        if(wt.size()<lookback()||wr.size()<lookback()) return NAN;
        double r_t=std::log(wt.back().close/wt[wt.size()-1-m_period].close);
        double r_r=std::log(wr.back().close/wr[wr.size()-1-m_period].close);
        return r_t - r_r;
    }

private:
    std::string m_ref_symbol;
    size_t m_period;
};

class RelativeMomentumFactory : public IFeatureFactory {
public:
    std::string family() const override { return "rel_momentum"; }
    std::unique_ptr<IFeature> create(const std::string& spec) override {
        // "rel_momentum_SPY_10"
        auto first = spec.find('_',0);
        auto second = spec.find('_',first+1);
        auto third = spec.find('_',second+1);
        std::string ref = spec.substr(second+1, third-second-1);
        size_t p = std::stoul(spec.substr(third+1));
        return std::make_unique<RelativeMomentumFeature>(ref,p);
    }
};
