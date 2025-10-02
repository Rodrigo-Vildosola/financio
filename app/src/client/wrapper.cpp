#include "financio/client/wrapper.h"


TradingWrapper::TradingWrapper(RingBuffer<TradingEvent, 1024>& evQueue)
    : m_ev_queue(evQueue) {}

void TradingWrapper::nextValidId(OrderId orderId) {
    TradingEvent ev{TradingEventType::Connected, 0, 0.0, 0};
    m_ev_queue.push(ev);
}

void TradingWrapper::connectionClosed() {
    TradingEvent ev{TradingEventType::Disconnected, 0, 0.0, 0};
    m_ev_queue.push(ev);
}

void TradingWrapper::error(int id, time_t errorTime, int errorCode, const std::string& errorString, const std::string& advancedOrderRejectJson) {
    TradingEvent ev{TradingEventType::Error, id, 0.0, errorCode};
    m_ev_queue.push(ev);
}

void TradingWrapper::tickPrice(TickerId tickerId, TickType field, double price,
                               const TickAttrib& attrib) {
    TradingEvent ev{TradingEventType::Tick, (int)tickerId, price, (int)field};
    m_ev_queue.push(ev);
}
