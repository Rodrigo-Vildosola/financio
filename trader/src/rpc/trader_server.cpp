#include "trader/rpc/trader_service.h"
#include <thread>

using namespace financio::trading;

TraderServiceImpl::TraderServiceImpl(TradingWorker& worker) : m_worker(worker) {}

grpc::Status TraderServiceImpl::Session(grpc::ServerContext*, grpc::ServerReaderWriter<TraderToUi, UiToTrader>* stream) {

    // Thread to push TradingEvents to UI
    std::atomic<bool> running{true};
    std::thread sender([&] {
        TradingEvent ev;
        while (running) {
            while (m_worker.pollEvent(ev)) {
                TraderToUi proto_out;
                translateEvent(ev, proto_out);
                stream->Write(proto_out);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    });

    // Read UI -> Trader requests
    UiToTrader proto_in;
    while (stream->Read(&proto_in)) {
        handleProtoRequest(proto_in);
    }

    running = false;
    sender.join();
    return grpc::Status::OK;
}

void TraderServiceImpl::handleProtoRequest(const UiToTrader& in) {
    if (!in.has_trading()) return;

    TradingRequest req{};
    if (translateRequest(in.trading(), req)) {
        m_worker.postRequest(req);
    }
}

bool TraderServiceImpl::translateRequest(const ClientRequest& msg, TradingRequest& out) {
    // simple example
    switch (msg.type()) {
        case REQUEST_TYPE_CONNECT: {
            const auto& pb = msg.connect();
            out.type = TradingRequestType::Connect;
            out.id = 0;
            out.payload = ConnectRequest{pb.host(), pb.port(), pb.client_id()};
            return true;
        }
        default:
            return false;
    }
}

void TraderServiceImpl::translateEvent(const TradingEvent& ev, TraderToUi& out) {
    auto* evt = out.mutable_event();
    evt->set_id(ev.id);
    evt->set_type(static_cast<int>(ev.type));
    // Fill details as needed
}
