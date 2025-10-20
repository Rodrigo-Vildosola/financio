#include "feature_vector.h"

#include <nlohmann/json.hpp>
#include <pybind11/embed.h>

namespace py = pybind11;

static py::dict to_pydict(const FeatureVector& f) {
    py::dict d;
    py::list xs;
    for (auto v : f.x) xs.append(v);
    d["x"] = xs;

    py::dict named;
    for (auto& kv : f.named) named[py::str(kv.first)] = kv.second;
    d["named"] = named;

    py::dict tags;
    for (auto& kv : f.tags) tags[py::str(kv.first)] = py::str(kv.second);
    d["tags"] = tags;

    d["symbol"] = py::str(f.symbol);
    d["timestamp"] = f.timestamp;
    return d;
}
