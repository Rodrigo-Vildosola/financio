#include "feature_extractor.h"
#include <algorithm>
#include <iostream>

FeatureExtractor::FeatureExtractor(BarFrequency freq) : m_frequency(freq) {}

void FeatureExtractor::register_factory(std::unique_ptr<IFeatureFactory> f) {
    m_factories[f->family()] = std::move(f);
}

void FeatureExtractor::add_bar(const MarketData& bar) {
    auto& win = m_windows[bar.symbol];
    win.push_back(bar);
    if (win.size() > m_max_lookback)
        win.pop_front();
}

FeatureVector FeatureExtractor::compute(const std::string& symbol, const std::vector<std::string>& requested) {
    FeatureVector fv;
    auto& w = m_windows[symbol];
    if (w.empty()) return fv;
    fv.symbol = symbol;
    fv.timestamp = w.back().timestamp;

    for (const auto& name : requested) {
        auto feat = get_or_create_feature(name);
        if (!feat) {
            std::cerr << "Warning: unknown feature '" << name << "'\n";
            fv.x.push_back(NAN);
            continue;
        }

        double val = feat->compute(m_windows, symbol);
        fv.named[name] = val;
        fv.x.push_back(val);
    }
    return fv;
}

void FeatureExtractor::reset() {
    m_windows.clear();
    m_features.clear();
    m_max_lookback = 20;
}

IFeature* FeatureExtractor::get_or_create_feature(const std::string& name) {
    auto it = m_features.find(name);
    if (it != m_features.end())
        return it->second.get();

    IFeatureFactory* matched_factory = nullptr;
    std::string matched_name;

    for (auto& [fam_name, factory] : m_factories) {
        if (name.rfind(fam_name, 0) == 0) {
            if (fam_name.size() > matched_name.size()) {
                matched_name = fam_name;
                matched_factory = factory.get();
            }
        }
    }

    if (!matched_factory)
        return nullptr;

    auto feat = matched_factory->create(name);
    if (!feat)
        return nullptr;

    m_max_lookback = std::max(m_max_lookback, feat->lookback());

    auto ptr = feat.get();
    m_features[name] = std::move(feat);
    return ptr;
}
