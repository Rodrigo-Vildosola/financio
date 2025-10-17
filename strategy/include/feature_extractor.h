#pragma once

#include "feature_vector.h"
#include "features/feature_base.h"
#include <unordered_map>
#include <deque>
#include <string>
#include <memory>
#include <vector>

enum class BarFrequency { 
    Minute1, 
    Minute5, 
    Hour1, 
    Day1 
};

class MultiAssetFeatureExtractor {
public:
    explicit MultiAssetFeatureExtractor(BarFrequency freq = BarFrequency::Day1);

    void register_factory(std::unique_ptr<IFeatureFactory> f);
    void add_bar(const MarketData& bar);
    FeatureVector compute(const std::string& symbol, const std::vector<std::string>& requested);
    void reset();

private:
    BarFrequency m_frequency;
    size_t m_max_lookback = 20;
    std::unordered_map<std::string, std::deque<MarketData>> m_windows;
    std::unordered_map<std::string, std::unique_ptr<IFeatureFactory>> m_factories;
    std::unordered_map<std::string, std::unique_ptr<IFeature>> m_features;
};
