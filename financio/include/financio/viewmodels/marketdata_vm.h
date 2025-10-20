#pragma once

#include <unordered_map>
#include <string>

namespace app {

class MarketDataVM {
public:
    void set_price(const std::string& sym, double price) { m_prices[sym] = price; }
    double price(const std::string& sym) const {
        auto it = m_prices.find(sym);
        return it == m_prices.end() ? 0.0 : it->second;
    }

private:
    std::unordered_map<std::string, double> m_prices;
};

} // namespace app
