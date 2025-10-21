#include "feature_vector.h"
#include "python/return_model_python.h"
#include <nlohmann/json.hpp>
#include <iostream>

int main() {
    using json = nlohmann::json;

    json spec = {
        {"environment", "Research"},
        {"strategy", {
            {"import", "strategies.flexible_strategy"},
            {"class", "FlexibleStrategy"},
            {"init", {
                {"combine", "mean"},
                {"return_models", {
                    {
                        {"import", "models.return_models"},
                        {"class", "LinearReturnModel"},
                        {"init", {{"w", {0.3, -0.1, 0.2}}, {"b", 0.0}}}
                    },
                    {
                        {"import", "models.return_models"},
                        {"class", "LinearReturnModel"},
                        {"init", {{"w", {0.3, -0.1, 0.2}}, {"b", 0.0}}}
                    }
                }},
                {"risk_models", {
                    {
                        {"import", "models.risk_models"},
                        {"class", "VolatilityRiskModel"},
                        {"init", {{"lambda_", 0.94}}}
                    }
                }}
            }}
        }},
        {"execution_model", {
            {"import", "execution.vwap"},
            {"class", "VWAPExecution"},
            {"init", {{"window", 10}}}
        }}
    };

    std::cout << spec.dump(2) << "\n";

    try {
        // === Create the engine ===
        auto engine = EngineFactory::create(spec);
        engine->load(spec);
        engine->initialize();

        // === Run the research simulation ===
        engine->run();
        engine->finalize();

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
