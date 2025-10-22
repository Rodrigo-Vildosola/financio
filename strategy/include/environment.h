// environment.h
#pragma once

#include "nlohmann/json.hpp"
#include <string>

enum class Environment {
    Research,
    Backtest,
    Simulation,
    Paper,
    Live
};

struct Context {
    Environment env;
    std::string symbol;
    long long timestamp;
    double portfolio_notional;
    bool training_mode{false};
};


inline constexpr const char* to_string(Environment e) {
    switch (e) {
        case Environment::Research: return "Research";
        case Environment::Backtest: return "Backtest";
        case Environment::Simulation: return "Simulation";
        case Environment::Paper: return "Paper";
        case Environment::Live: return "Live";
        default: return "Unknown";
    }
}

inline nlohmann::json to_json(const Context& ctx) {
    return {
        {"env", to_string(ctx.env)},
        {"symbol", ctx.symbol},
        {"timestamp", ctx.timestamp},
        {"portfolio_notional", ctx.portfolio_notional},
        {"training_mode", ctx.training_mode}
    };
}
