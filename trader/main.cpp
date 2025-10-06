#include <csignal>
#include <atomic>
#include <thread>
#include <iostream>
#include <chrono>
#include <grpcpp/grpcpp.h>

#include "trader/broker/trading_worker.h"
#include "trader/rpc/trader_service.h"

#include <eng/enginio.h>

static std::atomic<bool> running{true};

void handle_signal(int) {
    running = false;
    std::cout << "\n[Trader] Received shutdown signal...\n";
}

int main(int argc, char** argv) {
    // ------------------------------------------------------------
    // Setup: graceful shutdown handlers
    // ------------------------------------------------------------
    std::signal(SIGINT, handle_signal);
    std::signal(SIGTERM, handle_signal);

    std::cout << "=========================================\n";
    std::cout << "     Financio Trader Backend Service     \n";
    std::cout << "=========================================\n";

    // ------------------------------------------------------------
    // Start trading worker (IBKR interaction thread)
    // ------------------------------------------------------------
    std::cout << "[Trader] Starting trading worker...\n";
    trader::TradingWorker worker;
    worker.start();

    // ------------------------------------------------------------
    // Configure and start gRPC server
    // ------------------------------------------------------------
    const std::string server_address = "0.0.0.0:50051";
    trader::TraderServiceImpl service(worker);

    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    if (!server) {
        std::cerr << "[Trader] Failed to start gRPC server.\n";
        return 1;
    }

    std::cout << "[Trader] gRPC server listening on " << server_address << "\n";
    std::cout << "[Trader] Press Ctrl+C to shut down.\n";

    // ------------------------------------------------------------
    // Background thread: monitor running state
    // ------------------------------------------------------------
    std::thread monitor([&] {
        while (running) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        std::cout << "[Trader] Shutdown requested. Stopping server...\n";
        server->Shutdown();
    });

    // ------------------------------------------------------------
    // Block until server is stopped
    // ------------------------------------------------------------
    server->Wait();

    // ------------------------------------------------------------
    // Cleanup
    // ------------------------------------------------------------
    if (monitor.joinable())
        monitor.join();

    std::cout << "[Trader] Stopping worker...\n";
    worker.stop();

    std::cout << "[Trader] Shutdown complete.\n";
    return 0;
}
