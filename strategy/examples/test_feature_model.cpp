#include "feature_extractor.h"
#include "features/return_feature.h"
#include "features/momentum_feature.h"
#include "features/volatility_feature.h"
#include "features/relative_momentum_feature.h"
#include "feature_vector.h"
#include "python/return_model_python.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <iomanip>
#include <cmath>

int main() {
    using json = nlohmann::json;

    // 1. Build a simple linear Python model
    json spec = {
        {"class", "LinearReturnModel"},
        {"source", R"(
class LinearReturnModel:
    def __init__(self, w=None, b=0.0):
        self.w = w or []
        self.b = b
    def predict(self, features):
        x = features.get('x', [])
        s = sum(w*v for w,v in zip(self.w,x))
        return s + self.b
)"},
        // Weight order corresponds to requested features
        {"init", {{"w", {0.4, 0.3, 0.2, -0.1}}, {"b", 0.0}}}
    };

    Py_ReturnModel model;
    model.load(spec);

    // 2. Initialize feature extractor and register feature families
    MultiAssetFeatureExtractor extractor;
    extractor.register_factory(std::make_unique<ReturnFeatureFactory>());
    extractor.register_factory(std::make_unique<MomentumFeatureFactory>());
    extractor.register_factory(std::make_unique<VolatilityFeatureFactory>());
    extractor.register_factory(std::make_unique<RelativeMomentumFactory>());

    // 3. Create synthetic time series for AAPL and SPY
    constexpr int num_bars = 30;
    for (int i = 0; i < num_bars; ++i) {
        MarketData bar_aapl{
            "AAPL", 0.0, 0.0, 0.0,
            100.0 + 2.0 * std::sin(i / 3.0),
            1'000.0, i
        };
        MarketData bar_spy{
            "SPY", 0.0, 0.0, 0.0,
            50.0 + 0.5 * i,
            1'000.0, i
        };
        extractor.add_bar(bar_aapl);
        extractor.add_bar(bar_spy);

        // 4. Compute feature vector for AAPL
        FeatureVector f = extractor.compute(
            "AAPL",
            {"return_1", "momentum_5", "volatility_10", "rel_momentum_SPY_5"}
        );

        if (f.x.empty()) continue;

        // 5. Predict expected return μ using the Python model
        double mu = model.predict(f);

        // 6. Display features and model output
        std::cout << std::fixed << std::setprecision(6)
                  << "t=" << i
                  << " μ_pred=" << mu
                  << " features=" << f.to_json().dump()
                  << '\n';
    }

    std::cout << "Feature extraction and prediction complete.\n";
    return 0;
}
