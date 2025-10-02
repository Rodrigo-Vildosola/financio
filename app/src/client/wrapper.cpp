#include "financio/client/wrapper.h"


TradingWrapper::TradingWrapper(RingBuffer<TradingEvent, 1024>& evQueue) : m_ev_queue(evQueue) {}

void TradingWrapper::nextValidId(OrderId orderId) {
    TradingEvent ev{};
    ev.type = TradingEventType::Connected;
    ev.id   = static_cast<int>(orderId);
    m_ev_queue.push(ev);
}

void TradingWrapper::connectionClosed() {
    TradingEvent ev{};
    ev.type = TradingEventType::Disconnected;
    ev.id   = 0;
    m_ev_queue.push(ev);
}


void TradingWrapper::error(int id, time_t /*errorTime*/, int errorCode, const std::string& errorString, const std::string& /*advancedOrderRejectJson*/) {
    TradingEvent ev{};
    ev.type = TradingEventType::Error;
    ev.id   = id;
    ev.error.code = errorCode;
    std::strncpy(ev.error.message, errorString.c_str(), MSG_LEN - 1);
    ev.error.message[MSG_LEN - 1] = '\0';
    m_ev_queue.push(ev);
}

void TradingWrapper::tickPrice(TickerId tickerId, TickType field, double price,
                               const TickAttrib& attrib) {
    TradingEvent ev{TradingEventType::Tick, (int)tickerId, price, (int)field};
    m_ev_queue.push(ev);
}
