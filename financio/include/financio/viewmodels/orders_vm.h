#pragma once

#include <vector>
#include <string>

namespace app {

struct OrderInfo {
    std::string symbol;
    int quantity;
    double price;
    std::string status;
};

class OrdersVM {
public:
    void add_order(OrderInfo o) { m_orders.push_back(std::move(o)); }
    const std::vector<OrderInfo>& orders() const { return m_orders; }

private:
    std::vector<OrderInfo> m_orders;
};

} // namespace app
