#include "financio/rpc/trader_client.h"
#include <iostream>

using namespace financio::trading;

namespace app {

TraderClient::TraderClient(const std::string& address) {
    auto channel = grpc::CreateChannel(address, grpc::InsecureChannelCredentials());
    m_stub = TradingLink::NewStub(channel);
}

TraderClient::~TraderClient() {
    disconnect();
}

bool TraderClient::connect() {
    m_context = std::make_unique<grpc::ClientContext>();
    m_stream = m_stub->Session(m_context.get());
    m_running = true;
    m_reader_thread = std::thread(&TraderClient::reader_loop, this);
    return true;
}

void TraderClient::disconnect() {
    if (!m_running) return;
    m_running = false;

    m_context->TryCancel();
    if (m_reader_thread.joinable())
        m_reader_thread.join();
}

bool TraderClient::send_control(const ControlMessage& msg) {
    if (!m_stream) return false;
    return m_stream->Write(msg);
}

void TraderClient::set_state_handler(StateHandler handler) {
    m_onState = std::move(handler);
}

void TraderClient::reader_loop() {
    StateMessage msg;
    while (m_running && m_stream->Read(&msg)) {
        if (m_onState) m_onState(msg);
    }

    grpc::Status status = m_stream->Finish();
    std::cout << "[TraderClient] Stream closed: " << status.error_message() << "\n";
}

} // namespace app
