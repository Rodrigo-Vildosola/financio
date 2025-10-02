
#include "financio/client/ring_buffer.h"
#include "financio/client/states.h"

// IB API headers
#include <twsapi/EClientSocket.h>
#include <twsapi/EWrapper.h>
#include <twsapi/EReader.h>
#include <twsapi/Contract.h>

class TradingWrapper : public EWrapper {
public:
    TradingWrapper(RingBuffer<TradingEvent, 1024>& evQueue);

    void nextValidId(OrderId orderId) override;
    void connectionClosed() override;
    void error(int id, time_t errorTime, int errorCode, const std::string& errorString, const std::string& advancedOrderRejectJson) override;
    void tickPrice(TickerId tickerId, TickType field, double price, const TickAttrib& attrib) override;

private:
    RingBuffer<TradingEvent, 1024>& m_ev_queue;
};
