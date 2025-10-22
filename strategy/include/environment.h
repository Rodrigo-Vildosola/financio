// environment.h
#pragma once

#include <string>

enum class Environment {
    Research,
    Backtest,
    Simulation,
    Paper,
    Live
};

class Context {
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
