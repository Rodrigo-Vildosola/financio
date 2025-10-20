#include "feature_vector.h"
#include "python/return_model_python.h"
#include <nlohmann/json.hpp>
#include <iostream>

int main() {
    using json = nlohmann::json;

    json spec = {
        {"import", "models.return_models"},   // module path under python/
        {"class", "LinearReturnModel"},
        {"init", {{"w", {0.4, 0.3, 0.2, -0.1}}, {"b", 0.0}}}
    };

    Py_ReturnModel m;
    m.load(spec);

    FeatureVector f;
    f.x = {0.02, -0.01, 0.005};
    f.symbol = "AAPL";

    double mu = m.predict(f);
    std::cout << "Predicted μ = " << mu << "\n";
}
