#include "engines/research_engine.h"

#include "feature_extractor.h"
#include "features/momentum_feature.h"
#include "features/return_feature.h"
#include "features/volatility_feature.h"

void ResearchEngine::initialize() {
    // Strategy is the single Python entry point.
    if (!m_spec.contains("strategy"))
        throw std::runtime_error("ResearchEngine: missing 'strategy' spec");

    m_strategy = std::make_unique<Py_Strategy>();
    m_strategy->load(m_spec.at("strategy"));

    // Feature extractor is optional utility.
    m_extractor = std::make_unique<FeatureExtractor>();
    m_extractor->register_factory(std::make_unique<ReturnFeatureFactory>());
    m_extractor->register_factory(std::make_unique<MomentumFeatureFactory>());
    m_extractor->register_factory(std::make_unique<VolatilityFeatureFactory>());
}
