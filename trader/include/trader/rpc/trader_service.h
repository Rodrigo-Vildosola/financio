#pragma once

#include <grpcpp/grpcpp.h>
#include "trading/control.pb.h"
#include "trading/state.pb.h"
#include "trading/trader_service.grpc.pb.h"
#include "trader/broker/trading_worker.h"
#include "trading/trader_service.pb.h"

namespace trader {

class TraderServiceImpl final : public financio::trading::TradingLink::Service {
public:
    explicit TraderServiceImpl(TradingWorker& worker);

    grpc::Status Session(grpc::ServerContext* context, grpc::ServerReaderWriter<financio::trading::StateMessage, financio::trading::ControlMessage>* stream) override;

private:
    TradingWorker& m_worker;

    void handle_inbound(const financio::trading::ControlMessage& in);

    void send_state(const financio::trading::StateMessage& ev, financio::trading::StateMessage& out);

};

}
