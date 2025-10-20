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
    std::string family() const override { 
        return "rel_momentum"; 
    }

    std::unique_ptr<IFeature> create(const std::string& spec) override {
        // Expected format: "rel_momentum_<ref>_<period>"
        auto last_uscore = spec.rfind('_');
        if (last_uscore == std::string::npos) return nullptr;
        auto second_last = spec.rfind('_', last_uscore - 1);
        if (second_last == std::string::npos) return nullptr;

        std::string ref = spec.substr(second_last + 1, last_uscore - second_last - 1);
        size_t p = std::stoul(spec.substr(last_uscore + 1));
        return std::make_unique<RelativeMomentumFeature>(ref, p);
    }
};
