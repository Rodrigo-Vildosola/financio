#include "feature_extractor.h"
#include "features/return_feature.h"
#include "features/momentum_feature.h"
#include "features/volatility_feature.h"
#include "features/relative_momentum_feature.h"
#include <iostream>
#include <iomanip>

int main() {
    MultiAssetFeatureExtractor extractor;

    // Register available feature families
    extractor.register_factory(std::make_unique<ReturnFeatureFactory>());
    extractor.register_factory(std::make_unique<MomentumFeatureFactory>());
    extractor.register_factory(std::make_unique<VolatilityFeatureFactory>());
    extractor.register_factory(std::make_unique<RelativeMomentumFactory>());

    // Generate synthetic bars (AAPL, SPY) with incremental close prices
    constexpr int num_bars = 20;
    for (int i = 0; i < num_bars; ++i) {
        MarketData bar_aapl{
            "AAPL", 
            0.0, 
            0.0, 
            0.0, 
            static_cast<double>(100 + 2 * std::sin(i/2.0)), 
            1'000.0, 
            i
        };
        MarketData bar_spy {
            "SPY",  
            0.0, 
            0.0, 
            0.0, 
            static_cast<double>(50  + i), 
            1'000.0, 
            i
        };
        extractor.add_bar(bar_aapl);
        extractor.add_bar(bar_spy);

        // Compute features for AAPL only
        FeatureVector fv = extractor.compute(
            "AAPL",
            {"return_1", "momentum_5", "volatility_10", "rel_momentum_SPY_5"}
        );

        if (!fv.x.empty()) {
            std::cout << std::fixed << std::setprecision(6) << fv.to_json().dump() << '\n';
        }
    }

    std::cout << "Feature extraction complete.\n";
    return 0;
}
