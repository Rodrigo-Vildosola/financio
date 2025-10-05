#include "financio/client/trading_wrapper.h"


TradingWrapper::TradingWrapper(RingBuffer<TradingEvent, 1024>& evQueue) : m_ev_queue(evQueue) {}

// ---- Connection ----
void TradingWrapper::nextValidId(OrderId orderId) {
    TradingEvent ev{};
    ev.type = TradingEventType::Connected;
    ev.id   = static_cast<i32>(orderId);
    ev.payload = {};
    m_ev_queue.push(ev);
}

void TradingWrapper::connectionClosed() {
    TradingEvent ev{};
    ev.type = TradingEventType::Disconnected;
    ev.id   = 0;
    ev.payload = {};
    m_ev_queue.push(ev);
}

// ---- Errors ----
void TradingWrapper::error(i32 id, time_t /*errorTime*/, i32 errorCode, const std::string& errorString, const std::string& /*advancedOrderRejectJson*/) {
    TradingEvent ev{};
    ev.type = TradingEventType::Error;
    ev.id   = id;
    ev.payload = ErrorData{
        .code = errorCode,
        .message = errorString
    };
    m_ev_queue.push(ev);
}

void TradingWrapper::tickPrice(TickerId tickerId, TickType field, f64 price, const TickAttrib& attrib) {
    TradingEvent ev{};
    ev.type = TradingEventType::TickPrice;
    ev.id   = static_cast<i32>(tickerId);
    ev.payload = TickPriceData{
        .price = price,
        .field = static_cast<i32>(field),
        .attrib = attrib
    };
    m_ev_queue.push(ev);
}

void TradingWrapper::tickSize(TickerId tickerId, TickType field, Decimal size) {
    TradingEvent ev{};
    ev.type = TradingEventType::TickSize;
    ev.id   = static_cast<i32>(tickerId);
    ev.payload = TickSizeData{
        .field = static_cast<i32>(field),
        .size  = DecimalFunctions::decimalToDouble(size)
    };
    m_ev_queue.push(ev);
}


// ---- Orders ----
void TradingWrapper::orderStatus(OrderId orderId, const std::string& status,
                                 Decimal filled, Decimal remaining,
                                 f64 avgFillPrice, i64 /*permId*/, i32 /*parentId*/,
                                 f64 /*lastFillPrice*/, i32 /*clientId*/,
                                 const std::string& /*whyHeld*/, f64 /*mktCapPrice*/) {
    TradingEvent ev{};
    ev.type = TradingEventType::OrderStatus;
    ev.id   = static_cast<i32>(orderId);
    ev.payload = OrderStatusData{
        .status = status,
        .filled = DecimalFunctions::decimalToDouble(filled),
        .remaining = DecimalFunctions::decimalToDouble(remaining),
        .avgFillPrice = avgFillPrice
    };
    m_ev_queue.push(ev);
}

void TradingWrapper::execDetails(i32 reqId, const Contract& contract, const Execution& execution) {
    TradingEvent ev{};
    ev.type = TradingEventType::OrderFilled;
    ev.id   = reqId;
    ev.payload = OrderFilledData{
        .exec = execution,
    };
    m_ev_queue.push(ev);
}

// ---- Historical ----
void TradingWrapper::historicalData(TickerId reqId, const Bar& bar) {
    TradingEvent ev{};
    ev.type = TradingEventType::HistoricalBar;
    ev.id   = static_cast<i32>(reqId);
    ev.payload = HistoricalBarData{
        .bar  = bar
    };
    m_ev_queue.push(ev);
}

void TradingWrapper::historicalDataEnd(i32 reqId, const std::string& start, const std::string& end) {
    TradingEvent ev{};
    ev.type = TradingEventType::HistoricalEnd;
    ev.id   = reqId;
    ev.payload = HistoricalEndData{
        .start = start,
        .end   = end
    };
    m_ev_queue.push(ev);
}
