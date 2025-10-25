#include "engines/research_engine.h"

#include "feature_extractor.h"
#include "features/momentum_feature.h"
#include "features/return_feature.h"
#include "features/volatility_feature.h"
#include "python/helpers.h"
#include "python/risk_model_python.h"
#include "python/return_model_python.h"
#include "python/strategy_python.h"
#include "python/execution_model_python.h"

#include <iomanip>
#include <cmath>
#include <iostream>

namespace py = pybind11;
using json = nlohmann::json;

/**
 * @brief Initialize all major components for the research engine.
 *        Loads the Python strategy, feature extractors, and sets environment flags.
 */
void ResearchEngine::initialize() {
    // --- Load Python strategy ---
    if (!m_spec.contains("strategy"))
        throw std::runtime_error("ResearchEngine: missing 'strategy' section in JSON spec");

    // --- build submodels ---
    const auto& strat_spec = m_spec.at("strategy");
    const auto& strat_init = strat_spec.value("init", json::object());

    instantiate_models(strat_init);


    // --- load Python strategy ---
    m_strategy = std::make_unique<Py_Strategy>();
    {
        py::gil_scoped_acquire gil;

        py::list py_returns, py_risks;
        for (auto& rm : m_return_models) py_returns.append(rm->instance());
        for (auto& rk : m_risk_models)  py_risks.append(rk->instance());

        py::dict kwargs;
        for (auto& [key, val] : strat_init.items())
            kwargs[py::str(key)] = pyhelpers::json_to_pyobject(val);

        kwargs["return_models"] = py_returns;
        kwargs["risk_models"]   = py_risks;

        m_strategy->load_with_kwargs(strat_spec, kwargs);
        m_strategy->set_environment(m_env);
    }


    // --- execution ---
    if (m_spec.contains("execution_model")) {
        m_exec = std::make_unique<Py_ExecutionModel>();
        m_exec->load(m_spec.at("execution_model"));
        m_exec->set_environment(m_env);
    }

    // --- feature extraction ---
    m_extractor = std::make_unique<FeatureExtractor>();
    m_extractor->register_factory(std::make_unique<ReturnFeatureFactory>());
    m_extractor->register_factory(std::make_unique<MomentumFeatureFactory>());
    m_extractor->register_factory(std::make_unique<VolatilityFeatureFactory>());

    std::cerr << "[ResearchEngine] Initialized (" << to_string(m_env) << ")\n";
}

void ResearchEngine::instantiate_models(const json& strat_init) {
    m_return_models.clear();
    m_risk_models.clear();

    for (auto& rm : strat_init.value("return_models", json::array())) {
        auto model = std::make_unique<Py_ReturnModel>();
        model->load(rm);
        model->set_environment(m_env);
        m_return_models.push_back(std::move(model));
    }

    for (auto& rk : strat_init.value("risk_models", json::array())) {
        auto model = std::make_unique<Py_RiskModel>();
        model->load(rk);
        model->set_environment(m_env);
        m_risk_models.push_back(std::move(model));
    }
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
        double price = 100.0 + 2.0 * std::sin(t / 3.0);
        MarketData bar{"AAPL", 0.0, 0.0, 0.0, price, 1'000.0, t};
        m_extractor->add_bar(bar);

        FeatureVector f = m_extractor->compute(
            "AAPL", {"return_1", "momentum_5", "volatility_10"});
        if (f.x.empty()) continue;

        Context ctx{m_env, f.symbol, f.timestamp, 1e6, false};

        double target_weight = m_strategy ? m_strategy->on_bar(f) : 0.0;

        double realized = (price - prev_price) / prev_price;
        prev_price = price;

        for (auto& rm : m_return_models) rm->update(f, realized);
        for (auto& rk : m_risk_models)  rk->update(f, realized);
        if (m_strategy) m_strategy->update(f, realized);

        if (m_exec) {
            auto orders = m_exec->generate_orders(target_weight, ctx);
            std::cout << "Orders: " << orders.dump() << "\n";
        }

        std::cout << std::setw(2) << t
                  << " | " << std::setw(7) << price
                  << " | " << std::setw(7) << target_weight
                  << " | " << std::setw(9) << realized
                  << "\n";
    }

    std::cout << "[ResearchEngine] Simulation complete.\n";
}
