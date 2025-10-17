#include "feature_extractor.h"
#include <algorithm>
#include <iostream>

MultiAssetFeatureExtractor::MultiAssetFeatureExtractor(BarFrequency freq) : m_frequency(freq) {}

void MultiAssetFeatureExtractor::register_factory(std::unique_ptr<IFeatureFactory> f) {
    m_factories[f->family()] = std::move(f);
}

void MultiAssetFeatureExtractor::add_bar(const MarketData& bar) {
    auto& win = m_windows[bar.symbol];
    win.push_back(bar);
    if (win.size() > m_max_lookback)
        win.pop_front();
}

FeatureVector MultiAssetFeatureExtractor::compute(const std::string& symbol, const std::vector<std::string>& requested) {
    FeatureVector fv;
    auto& w = m_windows[symbol];
    if (w.empty()) return fv;
    fv.symbol = symbol;
    fv.timestamp = w.back().timestamp;

    for (const auto& name : requested) {
        auto it_feat = m_features.find(name);
        if (it_feat == m_features.end()) {
            // dynamic creation
            auto family_end = name.find('_');
            std::string family = name.substr(0, family_end);
            auto it_fact = m_factories.find(family);
            if (it_fact != m_factories.end()) {
                auto feat = it_fact->second->create(name);
                if (feat) {
                    m_max_lookback = std::max(m_max_lookback, feat->lookback());
                    m_features[name] = std::move(feat);
                }
            }
            it_feat = m_features.find(name);
            if (it_feat == m_features.end()) continue;
        }

        double val = it_feat->second->compute(m_windows, symbol);
        fv.named[name] = val;
        fv.x.push_back(val);
    }
    return fv;
}

void MultiAssetFeatureExtractor::reset() {
    m_windows.clear();
    m_features.clear();
}
