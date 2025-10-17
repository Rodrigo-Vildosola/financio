#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <nlohmann/json.hpp>

struct FeatureVector {
    std::vector<double> x;
    std::unordered_map<std::string, double> named;
    std::unordered_map<std::string, std::string> tags;
    std::string symbol;
    long long timestamp{};

    nlohmann::json to_json() const {
        nlohmann::json j;
        j["x"] = x;
        j["named"] = named;
        j["tags"] = tags;
        j["symbol"] = symbol;
        j["timestamp"] = timestamp;
        return j;
    }
};
