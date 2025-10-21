#pragma once

#include "environment.h"

#include <string>

struct Context {
    Environment env;
    std::string symbol;
    long long timestamp;
    double portfolio_notional;
    bool training_mode{false};
};
