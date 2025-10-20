#include <csignal>
#include <atomic>
#include <thread>
#include <iostream>
#include <chrono>
#include <grpcpp/grpcpp.h>
#include "spdlog/logger.h"

#include "trader/core/logger.h"
#include "trader/broker/trading_worker.h"
#include "trader/rpc/trader_service.h"


static std::atomic<bool> g_running{true};

extern "C" void handle_signal(int sig) {
    TR_CORE_WARN("Signal {} received. Initiating shutdown...", sig);
    g_running.store(false, std::memory_order_relaxed);
}

int main(int argc, char** argv) {
    logger::init();
    TR_CORE_INFO("Trader process starting...");

    std::signal(SIGINT,  handle_signal);
    std::signal(SIGTERM, handle_signal);

    trader::TradingWorker worker;
    TR_CORE_INFO("Starting TradingWorker...");
    worker.start();

    trader::TraderServiceImpl service(worker);
    grpc::ServerBuilder builder;
    const std::string addr = "0.0.0.0:50051";
    builder.AddListeningPort(addr, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<grpc::Server> server = builder.BuildAndStart();
    if (!server) {
        TR_CORE_ERROR("Failed to start gRPC server on {}", addr);
        return 1;
    }
    TR_CORE_INFO("gRPC server listening on {}", addr);

    std::thread monitor([&]{
        TR_CORE_DEBUG("Monitor thread started.");
        while (g_running.load(std::memory_order_relaxed))
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        TR_CORE_WARN("Shutdown signal detected. Stopping server...");
        server->Shutdown();
    });

    server->Wait();
    TR_CORE_INFO("Server stopped. Waiting for monitor thread...");

    if (monitor.joinable()) monitor.join();

    TR_CORE_INFO("Stopping TradingWorker...");
    worker.stop();

    TR_CORE_INFO("Shutdown complete.");
    return 0;
}
