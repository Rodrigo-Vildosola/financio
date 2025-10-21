#include "engines/research_engine.h"

#include "feature_extractor.h"
#include "features/momentum_feature.h"
#include "features/return_feature.h"
#include "features/volatility_feature.h"
#include "python/strategy_python.h"
#include "python/execution_model_python.h"

#include <iomanip>
#include <cmath>
#include <iostream>

using json = nlohmann::json;

/**
 * @brief Initialize all major components for the research engine.
 *        Loads the Python strategy, feature extractors, and sets environment flags.
 */
void ResearchEngine::initialize() {
    // --- Load Python strategy ---
    if (!m_spec.contains("strategy"))
        throw std::runtime_error("ResearchEngine: missing 'strategy' section in JSON spec");

    m_strategy = std::make_unique<Py_Strategy>();
    m_strategy->load(m_spec.at("strategy"));
    m_strategy->set_environment(m_env);

    if (m_spec.contains("execution_model")) {
        m_exec = std::make_unique<Py_ExecutionModel>();
        m_exec->load(m_spec.at("execution_model"));
        m_exec->set_environment(m_env);
    }

    // --- Initialize feature extractor ---
    m_extractor = std::make_unique<FeatureExtractor>();
    m_extractor->register_factory(std::make_unique<ReturnFeatureFactory>());
    m_extractor->register_factory(std::make_unique<MomentumFeatureFactory>());
    m_extractor->register_factory(std::make_unique<VolatilityFeatureFactory>());

    std::cerr << "[ResearchEngine] Initialized with environment="
              << to_string(m_env) << "\n";
}

/**
 * @brief Execute the research loop: feed bars, extract features,
 *        invoke strategy.on_bar(), and perform updates.
 */
void ResearchEngine::run() {
    constexpr int N = 50;
    double prev_price = 100.0;

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "t | price | weight | realized\n";
    std::cout << "-----------------------------\n";

    for (int t = 0; t < N; ++t) {
        // Synthetic market data
        double price = 100.0 + 2.0 * std::sin(t / 3.0);
        MarketData bar{"AAPL", 0.0, 0.0, 0.0, price, 1'000.0, t};
        m_extractor->add_bar(bar);

        // Compute features for current symbol
        FeatureVector f = m_extractor->compute(
            "AAPL", {"return_1", "momentum_5", "volatility_10"});

        if (f.x.empty()) continue;

        // Strategy prediction
        double target_weight = m_strategy ? m_strategy->on_bar(f) : 0.0;

        // Realized return
        double realized = (price - prev_price) / prev_price;
        prev_price = price;

        // Update strategy (handles submodels internally)
        if (m_strategy) m_strategy->update(f, realized);

        std::cout << std::setw(2) << t
                  << " | " << std::setw(7) << price
                  << " | " << std::setw(7) << target_weight
                  << " | " << std::setw(9) << realized
                  << "\n";
    }

    std::cout << "[ResearchEngine] Simulation complete.\n";
}
