#pragma once

#include <atomic>
#include <string>

namespace app {

class ConnectionVM {
public:
    void set_status(bool s) { m_connected.store(s); }
    bool connected() const { return m_connected.load(); }
    std::string address = "localhost:50051";

private:
    std::atomic<bool> m_connected{false};
};

} // namespace app
