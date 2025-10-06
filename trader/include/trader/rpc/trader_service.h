#pragma once

#include <grpcpp/grpcpp.h>
#include "financio/trading/trader_service.grpc.pb.h"
#include "trader/client/trading_worker.h"

class TraderServiceImpl final : public financio::trading::Trader::Service {
public:
    explicit TraderServiceImpl(TradingWorker& worker);

    grpc::Status Session(grpc::ServerContext* context, grpc::ServerReaderWriter<financio::trading::TraderToUi, financio::trading::UiToTrader>* stream) override;

private:
    TradingWorker& m_worker;

    void handleProtoRequest(const financio::trading::UiToTrader& in);
    
    bool translateRequest(const financio::trading::ClientRequest& msg, TradingRequest& out);

    void translateEvent(const TradingEvent& ev, financio::trading::TraderToUi& out);
};
