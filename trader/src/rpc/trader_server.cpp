#include "trader/rpc/trader_service.h"
#include "trading/control.pb.h"
#include "trading/state.pb.h"
#include "trading/trader_service.pb.h"
#include <thread>

using namespace financio::trading;

namespace trader {

TraderServiceImpl::TraderServiceImpl(TradingWorker& worker) : m_worker(worker) {}

grpc::Status TraderServiceImpl::Session(grpc::ServerContext*, grpc::ServerReaderWriter<StateMessage, ControlMessage>* stream) {

    // Thread to push TradingEvents to UI
    std::atomic<bool> running{true};
    std::thread sender([&] {
        StateMessage msg;
        while (running) {
            while (m_worker.pollEvent(msg)) {
                StateMessage out;
                send_state(msg, out);
                stream->Write(out);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    });

    // Read UI -> Trader requests
    ControlMessage in;
    while (stream->Read(&in)) {
        handle_inbound(in);
    }

    running = false;
    sender.join();
    return grpc::Status::OK;
}

void TraderServiceImpl::handle_inbound(const ControlMessage& in) {
    m_worker.postRequest(in);
}

void TraderServiceImpl::send_state(const StateMessage& ev, StateMessage& out) {
    // For now, just forward 1:1.
    // This allows a future transformation or filtering step.
    out.CopyFrom(ev);
}

// bool TraderServiceImpl::translateRequest(const ClientRequest& msg, TradingRequest& out) {
//     // simple example
//     switch (msg.type()) {
//         case REQUEST_TYPE_CONNECT: {
//             const auto& pb = msg.connect_req();
//             out.type = TradingRequestType::Connect;
//             out.id = 0;
//             out.payload = ConnectRequest{pb.host(), pb.port(), pb.client_id()};
//             return true;
//         }
//         default:
//             return false;
//     }
// }

// void TraderServiceImpl::translateEvent(const StateMessage& ev, BackendToFrontend& out) {
//     auto* evt = out.mutable_event();
//     evt->set_id(ev.id);
//     evt->set_type(static_cast<::financio::trading::EventType>(ev.type));
//     // Fill details as needed
// }

}
