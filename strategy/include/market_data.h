#pragma once
#include <string>

struct MarketData {
    std::string symbol;
    double open{}, high{}, low{}, close{}, volume{};
    long long timestamp{};
};
