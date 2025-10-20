#pragma once

#include <string>

namespace app {

class SymbolSelector {
public:
    void on_draw();
    const std::string& symbol() const { return m_symbol; }

private:
    std::string m_symbol = "AAPL";
};

} // namespace app
