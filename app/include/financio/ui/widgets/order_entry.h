#pragma once

namespace app {

class OrderEntry {
public:
    void on_draw();

private:
    int m_quantity = 100;
    float m_price = 0.0f;
};

} // namespace app
