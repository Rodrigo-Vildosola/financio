#include "feature_vector.h"
#include "python/return_model_python.h"
#include <nlohmann/json.hpp>
#include <iostream>

int main() {
    using json = nlohmann::json;

    json spec = {
        {"class", "LinearReturnModel"},
        {"source", R"(
from model_base import ReturnModelBase

class LinearReturnModel(ReturnModelBase):
    def __init__(self, w=None, b=0.0):
        self.w = w or []
        self.b = b

    def predict(self, features):
        x = features.get('x', [])
        s = sum(w*v for w,v in zip(self.w,x))
        return s + self.b
)"},
        {"init", {{"w", {0.5, 0.3, -0.2}}, {"b", 0.01}}}
    };

    Py_ReturnModel m;
    m.load(spec);

    FeatureVector f;
    f.x = {0.02, -0.01, 0.005};
    f.symbol = "AAPL";

    double mu = m.predict(f);
    std::cout << "Predicted μ = " << mu << "\n";
}
