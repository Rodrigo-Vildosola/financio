// environment.h
#pragma once

enum class Environment {
    Research,
    Backtest,
    Simulation,
    Paper,
    Live
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
