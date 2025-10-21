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
                    {{"import", "models.return_models"}, {"class", "LinearReturnModel"},
                    {"init", {{"w", {0.3, -0.1, 0.2}}, {"b", 0.0}}}},
                    {{"import", "models.neural"}, {"class", "NeuralReturnModel"},
                    {"init", {{"path", "weights.pt"}}}}
                }},
                {"risk_models", {
                    {{"import", "models.risk_models"}, {"class", "VolatilityRiskModel"},
                    {"init", {{"lambda_", 0.94}}}}
                }}
            }}
        }}
    };

}


