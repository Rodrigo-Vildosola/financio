#include "feature_extractor.h"
#include "features/return_feature.h"
#include "features/momentum_feature.h"
#include "features/volatility_feature.h"
#include "feature_vector.h"

#include "python/return_model_python.h"
#include "python/risk_model_python.h"

#include <nlohmann/json.hpp>
#include <iostream>
#include <iomanip>
#include <cmath>

int main() {
    using json = nlohmann::json;

    // --- Return Model ---
    json spec_return = {
        {"import", "models.return_models"},
        {"class", "LinearReturnModel"},
        {"init", {{"w", {0.4, 0.3, 0.2, -0.1}}, {"b", 0.0}}}
    };
    Py_ReturnModel return_model;
    return_model.load(spec_return);

    // --- Risk Model ---
    json spec_risk = {
        {"import", "models.risk_models"},
        {"class", "ExponentialVolatility"},
        {"init", {{"lambda_", 0.94}}}
    };
    Py_RiskModel risk_model;
    risk_model.load(spec_risk);

    // --- Feature Extractor ---
    FeatureExtractor extractor;
    extractor.register_factory(std::make_unique<ReturnFeatureFactory>());
    extractor.register_factory(std::make_unique<MomentumFeatureFactory>());
    extractor.register_factory(std::make_unique<VolatilityFeatureFactory>());

    constexpr int num_bars = 50;
    double prev_price = 100.0;
    double realized_return = 0.0;

    std::cout << std::fixed << std::setprecision(6);
    std::cout << " t | price | mu_pred | sigma_est | realized_ret\n";
    std::cout << "-----------------------------------------------\n";

    for (int t = 0; t < num_bars; ++t) {
        // Synthetic single-asset data (AAPL)
        double price = 100.0 + 2.0 * std::sin(t / 3.0);
        MarketData bar{"AAPL", 0.0, 0.0, 0.0, price, 1'000.0, t};
        extractor.add_bar(bar);

        // Feature extraction
        FeatureVector f = extractor.compute("AAPL", {"return_1", "momentum_5", "volatility_10"});
        if (f.x.empty()) continue;

        // Model predictions
        double mu = return_model.predict(f);
        double sigma = risk_model.estimate(f);

        // Compute realized return for updating volatility model
        realized_return = (price - prev_price) / prev_price;
        prev_price = price;
        risk_model.update(f, realized_return);

        // Log
        std::cout << std::setw(2) << t
                  << " | " << std::setw(7) << price
                  << " | " << std::setw(8) << mu
                  << " | " << std::setw(10) << sigma
                  << " | " << std::setw(12) << realized_return
                  << "\n";
    }

    std::cout << "-----------------------------------------------\n";
    std::cout << "Feature extraction, μ prediction, and σ estimation complete.\n";
    return 0;
}
