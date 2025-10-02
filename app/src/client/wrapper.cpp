#include "financio/client/wrapper.h"


TradingWrapper::TradingWrapper(RingBuffer<TradingEvent, 1024>& evQueue) : m_ev_queue(evQueue) {}

void TradingWrapper::nextValidId(OrderId orderId) {
    TradingEvent ev{};
    ev.type = TradingEventType::Connected;
    ev.id   = static_cast<i32>(orderId);
    ev.payload = ErrorData{0, "Connected"}; // minimal payload
    m_ev_queue.push(ev);
}

void TradingWrapper::connectionClosed() {
    TradingEvent ev{};
    ev.type = TradingEventType::Disconnected;
    ev.id   = 0;
    ev.payload = ErrorData{0, "Connection closed"};
    m_ev_queue.push(ev);
}


void TradingWrapper::error(i32 id, time_t /*errorTime*/, i32 errorCode, const std::string& errorString, const std::string& /*advancedOrderRejectJson*/) {
    TradingEvent ev{};
    ev.type = TradingEventType::Error;
    ev.id   = id;
    ev.payload = ErrorData{errorCode, errorString};
    m_ev_queue.push(ev);
}

void TradingWrapper::tickPrice(TickerId tickerId, TickType field, f64 price, const TickAttrib& attrib) {
    TradingEvent ev{};
    ev.type = TradingEventType::TickPrice;
    ev.id   = static_cast<i32>(tickerId);
    ev.payload = TickPriceData{
        .price      = price,
        .field      = static_cast<i32>(field),
        .attribMask = attrib.canAutoExecute | (attrib.pastLimit << 1) | (attrib.preOpen << 2)
    };
    m_ev_queue.push(ev);
}
