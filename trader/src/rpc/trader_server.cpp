#include "trader/rpc/trader_service.h"
#include <thread>
#include <chrono>

using financio::trading::ControlMessage;
using financio::trading::StateMessage;

namespace trader {

TraderServiceImpl::TraderServiceImpl(TradingWorker& worker) : m_worker(worker) {}

grpc::Status TraderServiceImpl::Session(grpc::ServerContext* ctx, grpc::ServerReaderWriter<StateMessage, ControlMessage>* stream) {

    std::atomic<bool> alive{true};

    // sender: backend -> frontend
    std::thread sender([&]{
        StateMessage msg;
        while (alive.load(std::memory_order_relaxed)) {
            // drain queue
            while (alive.load(std::memory_order_relaxed) && m_worker.pollEvent(msg)) {
                // Write may fail after shutdown/cancel; stop if so
                if (!stream->Write(msg)) {
                    alive.store(false, std::memory_order_relaxed);
                    break;
                }
            }
            if (!alive.load(std::memory_order_relaxed)) break;
            // stop early if RPC cancelled
            if (ctx->IsCancelled()) {
                alive.store(false, std::memory_order_relaxed);
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });

    // receiver: frontend -> backend
    ControlMessage in;
    while (!ctx->IsCancelled() && stream->Read(&in)) {
        m_worker.postRequest(in);
    }

    // tear down
    alive.store(false, std::memory_order_relaxed);
    if (sender.joinable()) sender.join();

    // If server is shutting down, return CANCELLED so client exits cleanly
    if (ctx->IsCancelled()) return grpc::Status::CANCELLED;
    return grpc::Status::OK;
}

void TraderServiceImpl::handle_inbound(const ControlMessage& in) {
    m_worker.postRequest(in);
}

void TraderServiceImpl::send_state(const StateMessage& ev, StateMessage& out) {
    out.CopyFrom(ev); // keep hook if you need mapping later
}

} // namespace trader
