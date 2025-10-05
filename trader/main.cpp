#include "trader/client/trading_worker.h"
#include <csignal>
#include <atomic>
#include <thread>
#include <iostream>

static std::atomic<bool> running{true};

void handle_signal(int) {
    running = false;
}

int main() {
    std::signal(SIGINT, handle_signal);
    std::signal(SIGTERM, handle_signal);

    std::cout << "[Trader] starting worker...\n";

    TradingWorker worker;
    worker.start();

    // connect immediately (optional)
    TradingRequest connect_req;
    connect_req.type = TradingRequestType::Connect;
    connect_req.id   = 0;
    connect_req.payload = ConnectRequest{"127.0.0.1", 4002, 1};
    worker.postRequest(connect_req);

    std::cout << "[Trader] running, press Ctrl+C to exit\n";

    TradingEvent ev;
    while (running) {
        while (worker.pollEvent(ev)) {
            std::cout << "[Event] type=" << static_cast<int>(ev.type)
                      << " id=" << ev.id << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    std::cout << "[Trader] stopping...\n";
    worker.stop();
    std::cout << "[Trader] shutdown complete.\n";
    return 0;
}
