#include <csignal>
#include <atomic>
#include <thread>
#include <iostream>
#include <chrono>
#include <grpcpp/grpcpp.h>

#include "eng/core/logger.h"
#include "trader/broker/trading_worker.h"
#include "trader/rpc/trader_service.h"

#include <eng/enginio.h>

static std::atomic<bool> g_running{true};

extern "C" void handle_signal(int) {
    g_running.store(false, std::memory_order_relaxed); // no I/O here
}

int main(int argc, char** argv) {
    eng::logger::init();

    std::signal(SIGINT,  handle_signal);
    std::signal(SIGTERM, handle_signal);

    trader::TradingWorker worker;
    worker.start();

    // ENG_WARN("We are here");

    trader::TraderServiceImpl service(worker);
    grpc::ServerBuilder builder;
    const std::string addr = "0.0.0.0:50051";
    builder.AddListeningPort(addr, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<grpc::Server> server = builder.BuildAndStart();
    if (!server) return 1;

    // monitor thread requests shutdown when signal flips the flag
    std::thread monitor([&]{
        while (g_running.load(std::memory_order_relaxed))
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        server->Shutdown(); // wakes all active RPCs
    });

    server->Wait();        // blocks until Shutdown()

    if (monitor.joinable()) monitor.join();
    worker.stop();
    return 0;
}
