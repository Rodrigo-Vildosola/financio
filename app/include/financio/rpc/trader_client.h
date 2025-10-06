#pragma once

#include <grpcpp/grpcpp.h>
#include "trading/trader_service.grpc.pb.h"
#include "trading/control.pb.h"
#include "trading/state.pb.h"

#include <thread>
#include <atomic>
#include <functional>

namespace app {

class TraderClient {
public:
    using StateHandler = std::function<void(const financio::trading::StateMessage&)>;

    TraderClient(const std::string& address);
    ~TraderClient();

    bool connect();
    void disconnect();

    bool send_control(const financio::trading::ControlMessage& msg);

    void set_state_handler(StateHandler handler);

private:
    void reader_loop();

    std::unique_ptr<financio::trading::TradingLink::Stub> m_stub;
    std::unique_ptr<grpc::ClientReaderWriter<
        financio::trading::ControlMessage,
        financio::trading::StateMessage>> m_stream;

    std::unique_ptr<grpc::ClientContext> m_context;

    std::atomic<bool> m_running{false};
    std::thread m_reader_thread;

    StateHandler m_onState;
};

} // namespace app
