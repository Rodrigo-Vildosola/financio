
#include "financio/client/ring_buffer.h"
#include "financio/types/request_types.h"
#include "financio/types/event_types.h"

// IB API headers
#include <twsapi/EClientSocket.h>
#include <twsapi/EWrapper.h>
#include <twsapi/EReader.h>
#include <twsapi/Contract.h>

class TradingWrapper : public EWrapper {
public:
    TradingWrapper(RingBuffer<TradingEvent, 1024>& evQueue);

    // ---- Core connection lifecycle ----
    void nextValidId(OrderId orderId) override;
    void connectionClosed() override;

    // ---- Errors ----
    void error(i32 id, time_t errorTime, i32 errorCode, const std::string& errorString, const std::string& advancedOrderRejectJson) override;

    // ---- Market data ----
    void tickPrice(TickerId tickerId, TickType field, f64 price, const TickAttrib& attrib) override;
    void tickSize(TickerId tickerId, TickType field, Decimal size) override;

    // ---- Orders ----
    void orderStatus(OrderId orderId, const std::string& status, Decimal filled, Decimal remaining, f64 avgFillPrice, i64 permId, i32 parentId, f64 lastFillPrice, i32 clientId, const std::string& whyHeld, f64 mktCapPrice) override;
    void execDetails(i32 reqId, const Contract& contract, const Execution& execution) override;

    // ---- Historical data ----
    void historicalData(TickerId reqId, const Bar& bar) override;
    void historicalDataEnd(i32 reqId, const std::string& start, const std::string& end) override;


private:
    RingBuffer<TradingEvent, 1024>& m_ev_queue;
};
