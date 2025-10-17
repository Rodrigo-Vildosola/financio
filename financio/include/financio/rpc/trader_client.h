#pragma once
#include <grpcpp/grpcpp.h>
#include "trading/trader_service.grpc.pb.h"
#include "trading/control.pb.h"
#include "trading/state.pb.h"
#include "financio/core/blockingconcurrentqueue.h"

#include <thread>
#include <atomic>
#include <functional>
#include <chrono>
#include <random>

namespace app {

class TraderClient {
public:
    using StateHandler = std::function<void(const financio::trading::StateMessage&)>;
    using VoidHandler  = std::function<void()>;
    using ErrorHandler = std::function<void(const grpc::Status&)>;

    struct Options {
        std::string address = "localhost:50051";
        int keepalive_ms = 10'000;
        int keepalive_timeout_ms = 3'000;
        int backoff_initial_ms = 250;
        int backoff_max_ms = 10'000;
        size_t outbound_capacity = 4096; // soft capacity policy
        bool enable_grpc_keepalive = true;
        bool heartbeat_over_app = true; // send CONTROL_PING if true
        int heartbeat_period_ms = 2'000;
        int heartbeat_miss_threshold = 3;
    };

    explicit TraderClient(Options opt);
    ~TraderClient();

    // Queue a control message; returns false if queue is full (drop policy).
    bool enqueue(const financio::trading::ControlMessage& msg);

    // Start/stop the client.
    void start();
    void stop();

    // Callbacks
    void on_state(StateHandler cb) { m_onState = std::move(cb); }
    void on_connected(VoidHandler cb) { m_onConnected = std::move(cb); }
    void on_disconnected(VoidHandler cb) { m_onDisconnected = std::move(cb); }
    void on_error(ErrorHandler cb) { m_onError = std::move(cb); }

    bool connected() const { return m_connected.load(std::memory_order_acquire); }

    // Optional: called by app to re-issue subscriptions after reconnect.
    void set_resubscribe(std::function<void()> cb) { m_onResubscribe = std::move(cb); }

private:
    void io_thread();
    bool open_stream();      // create channel, stub, context, stream
    void close_stream();     // writesdone+cancel
    void reader_loop();
    void writer_loop();
    void heartbeat_loop();

    Options m_opt;

    std::shared_ptr<grpc::Channel> m_channel;
    std::unique_ptr<financio::trading::TradingLink::Stub> m_stub;
    std::unique_ptr<grpc::ClientContext> m_context;
    std::unique_ptr<grpc::ClientReaderWriter<
        financio::trading::ControlMessage,
        financio::trading::StateMessage>> m_stream;

    moodycamel::BlockingConcurrentQueue<financio::trading::ControlMessage> m_outbound;

    std::thread m_io;          // supervises connect/reconnect and spawns subthreads
    std::thread m_reader;
    std::thread m_writer;
    std::thread m_heartbeat;

    std::atomic<bool> m_running{false};
    std::atomic<bool> m_connected{false};
    std::atomic<int>  m_missed_heartbeats{0};

    StateHandler m_onState;
    VoidHandler  m_onConnected;
    VoidHandler  m_onDisconnected;
    ErrorHandler m_onError;
    std::function<void()> m_onResubscribe;
};

} // namespace app
