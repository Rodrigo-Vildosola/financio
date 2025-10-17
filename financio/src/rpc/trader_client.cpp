#include "financio/rpc/trader_client.h"
#include <spdlog/spdlog.h>

using namespace financio::trading;
using namespace std::chrono;

namespace app {

static std::shared_ptr<grpc::Channel> make_channel(const TraderClient::Options& opt) {
    grpc::ChannelArguments args;
    if (opt.enable_grpc_keepalive) {
        args.SetInt(GRPC_ARG_KEEPALIVE_TIME_MS, opt.keepalive_ms);
        args.SetInt(GRPC_ARG_KEEPALIVE_TIMEOUT_MS, opt.keepalive_timeout_ms);
        args.SetInt(GRPC_ARG_HTTP2_MIN_SENT_PING_INTERVAL_WITHOUT_DATA_MS, opt.keepalive_ms);
        args.SetInt(GRPC_ARG_KEEPALIVE_PERMIT_WITHOUT_CALLS, 1);
    }
    args.SetInt(GRPC_ARG_MAX_RECONNECT_BACKOFF_MS, opt.backoff_max_ms);
    return grpc::CreateCustomChannel(opt.address, grpc::InsecureChannelCredentials(), args);
}

TraderClient::TraderClient(Options opt)
    : m_opt(std::move(opt)), m_outbound()
{}

TraderClient::~TraderClient() { stop(); }

bool TraderClient::enqueue(const ControlMessage& msg) {
    // Soft capacity guard: if queue is very large, drop with warning.
    if (m_outbound.size_approx() > m_opt.outbound_capacity) {
        spdlog::warn("TraderClient: outbound queue overflow, dropping msg id={}", msg.id());
        return false;
    }
    m_outbound.enqueue(msg);
    return true;
}

void TraderClient::start() {
    if (m_running.exchange(true)) return;
    m_io = std::thread(&TraderClient::io_thread, this);
}

void TraderClient::stop() {
    if (!m_running.exchange(false)) return;

    // Wake up all blocking waits.
    m_outbound.enqueue(ControlMessage{}); // sentinel no-op

    close_stream();

    if (m_heartbeat.joinable()) m_heartbeat.join();
    if (m_reader.joinable())    m_reader.join();
    if (m_writer.joinable())    m_writer.join();
    if (m_io.joinable())        m_io.join();
}

bool TraderClient::open_stream() {
    m_context = std::make_unique<grpc::ClientContext>();
    if (!m_channel) m_channel = make_channel(m_opt);
    m_stub = TradingLink::NewStub(m_channel);
    m_stream = m_stub->Session(m_context.get());
    if (!m_stream) return false;
    m_connected.store(true, std::memory_order_release);
    if (m_onConnected) m_onConnected();
    if (m_onResubscribe) m_onResubscribe();
    return true;
}

void TraderClient::close_stream() {
    if (m_stream) m_stream->WritesDone();
    if (m_context) m_context->TryCancel();
}

void TraderClient::io_thread() {
    std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<int> jitter(0, 250);

    int backoff = m_opt.backoff_initial_ms;
    while (m_running.load(std::memory_order_acquire)) {
        if (!open_stream()) {
            std::this_thread::sleep_for(milliseconds(backoff + jitter(rng)));
            backoff = std::min(backoff * 2, m_opt.backoff_max_ms);
            continue;
        }
        backoff = m_opt.backoff_initial_ms;

        // Spawn reader, writer, heartbeat
        m_missed_heartbeats.store(0);
        m_reader   = std::thread(&TraderClient::reader_loop, this);
        m_writer   = std::thread(&TraderClient::writer_loop, this);
        if (m_opt.heartbeat_over_app) m_heartbeat = std::thread(&TraderClient::heartbeat_loop, this);

        // Wait until any subthread ends → treat as disconnect
        if (m_reader.joinable())   m_reader.join();
        if (m_writer.joinable())   m_writer.join();
        if (m_heartbeat.joinable()) m_heartbeat.join();

        // Close and signal upper layers
        grpc::Status status = grpc::Status::OK;
        if (m_stream) status = m_stream->Finish();
        m_stream.reset();
        m_context.reset();

        bool was_connected = m_connected.exchange(false);
        if (was_connected && m_onDisconnected) m_onDisconnected();
        if (!status.ok() && m_onError) m_onError(status);

        if (!m_running.load()) break;

        std::this_thread::sleep_for(milliseconds(backoff + jitter(rng)));
        backoff = std::min(backoff * 2, m_opt.backoff_max_ms);
    }
}

void TraderClient::reader_loop() {
    StateMessage msg;
    while (m_running.load() && m_stream && m_stream->Read(&msg)) {
        if (m_onState) m_onState(msg);
        // Reset heartbeat on any inbound state
        m_missed_heartbeats.store(0, std::memory_order_relaxed);
    }
}

void TraderClient::writer_loop() {
    ControlMessage msg;
    while (m_running.load() && m_stream) {
        // Blocks until a message is available or wakeup sentinel arrives
        m_outbound.wait_dequeue(msg);
        if (!m_running.load() || !m_stream) break;
        // Ignore default-constructed sentinel
        if (msg.type() == financio::trading::CONTROL_UNSPECIFIED) continue;
        if (!m_stream->Write(msg)) {
            // Write failed → break to trigger reconnect
            break;
        }
    }
}

void TraderClient::heartbeat_loop() {
    const auto period = milliseconds(m_opt.heartbeat_period_ms);
    while (m_running.load() && m_stream) {
        std::this_thread::sleep_for(period);
        if (!m_running.load() || !m_stream) break;

        ControlMessage ping;
        ping.set_type(CONTROL_PING);    // assumes your proto defines it
        ping.set_id(0);
        enqueue(ping);

        int missed = m_missed_heartbeats.fetch_add(1) + 1;
        if (missed >= m_opt.heartbeat_miss_threshold) {
            spdlog::warn("TraderClient: heartbeat missed {} times, forcing reconnect", missed);
            // Force reconnect by breaking writer/reader via cancel
            close_stream();
            break;
        }
    }
}

} // namespace app
