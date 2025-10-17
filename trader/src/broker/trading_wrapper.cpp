#include "trader/broker/trading_wrapper.h"

#include <twsapi/Execution.h>
#include "trading/state.pb.h"


using namespace financio::trading;

namespace trader {

static google::protobuf::Timestamp make_timestamp() {
    using namespace std::chrono;
    auto now = system_clock::now();
    auto secs = time_point_cast<seconds>(now);
    auto nanos = duration_cast<nanoseconds>(now - secs);
    google::protobuf::Timestamp ts;
    ts.set_seconds(secs.time_since_epoch().count());
    ts.set_nanos((int32_t)nanos.count());
    return ts;
}


TradingWrapper::TradingWrapper(RingBuffer<StateMessage, 1024>& s_queue) : m_state_queue(s_queue) {}

// ---- Connection ----
void TradingWrapper::nextValidId(OrderId orderId) {
    StateMessage msg;
    msg.set_type(STATE_CONNECTED);
    msg.set_id(static_cast<int64_t>(orderId));
    *msg.mutable_ts() = make_timestamp();
    m_state_queue.push(msg);
}

void TradingWrapper::connectionClosed() {
    StateMessage msg;
    msg.set_type(STATE_DISCONNECTED);
    msg.set_id(0);
    *msg.mutable_ts() = make_timestamp();
    m_state_queue.push(msg);
}

// ---- Errors ----
void TradingWrapper::error(i32 id, time_t /*errorTime*/, i32 errorCode, const std::string& errorString, const std::string& /*advancedOrderRejectJson*/) {
    StateMessage msg;
    msg.set_type(STATE_ERROR);
    msg.set_id(id);
    *msg.mutable_ts() = make_timestamp();

    auto* err = msg.mutable_error();
    err->set_code(errorCode);
    err->set_message(errorString);

    m_state_queue.push(msg);
}

void TradingWrapper::tickPrice(TickerId tickerId, TickType field, f64 price, const TickAttrib& attrib) {
    StateMessage msg;
    msg.set_type(STATE_TICK_PRICE);
    msg.set_id(static_cast<int64_t>(tickerId));
    *msg.mutable_ts() = make_timestamp();

    auto* tick = msg.mutable_tick_price();
    tick->set_price(price);
    tick->set_field(static_cast<int32_t>(field));

    auto* pb_attr = tick->mutable_attrib();
    pb_attr->set_can_auto_execute(attrib.canAutoExecute);
    pb_attr->set_past_limit(attrib.pastLimit);
    pb_attr->set_pre_open(attrib.preOpen);

    m_state_queue.push(msg);
}

void TradingWrapper::tickSize(TickerId tickerId, TickType field, Decimal size) {
    StateMessage msg;
    msg.set_type(STATE_TICK_SIZE);
    msg.set_id(static_cast<int64_t>(tickerId));
    *msg.mutable_ts() = make_timestamp();

    auto* tick = msg.mutable_tick_size();
    tick->set_field(static_cast<int32_t>(field));
    tick->set_size(DecimalFunctions::decimalToDouble(size));

    m_state_queue.push(msg);
}


// ---- Orders ----
void TradingWrapper::orderStatus(OrderId orderId, const std::string& status,
                                 Decimal filled, Decimal remaining,
                                 f64 avgFillPrice, i64 /*permId*/, i32 /*parentId*/,
                                 f64 /*lastFillPrice*/, i32 /*clientId*/,
                                 const std::string& /*whyHeld*/, f64 /*mktCapPrice*/) {
    StateMessage msg;
    msg.set_type(STATE_ORDER_STATUS);
    msg.set_id(static_cast<int64_t>(orderId));
    *msg.mutable_ts() = make_timestamp();

    auto* pb = msg.mutable_order_status();
    pb->set_status(status);
    pb->set_filled(DecimalFunctions::decimalToDouble(filled));
    pb->set_remaining(DecimalFunctions::decimalToDouble(remaining));
    pb->set_avg_fill_px(avgFillPrice);

    m_state_queue.push(msg);
}

void TradingWrapper::execDetails(int32_t reqId, const Contract& contract, const Execution& execution) {
    StateMessage msg;
    msg.set_type(STATE_ORDER_FILLED);
    msg.set_id(reqId);
    *msg.mutable_ts() = make_timestamp();

    auto* pb = msg.mutable_order_filled();
    auto* exec = pb->mutable_exec();

    exec->set_exec_id(execution.execId);
    exec->set_time(execution.time);
    exec->set_acct_number(execution.acctNumber);
    exec->set_exchange(execution.exchange);
    exec->set_side(execution.side);
    exec->set_shares(DecimalFunctions::decimalToDouble(execution.shares));
    exec->set_price(execution.price);
    exec->set_perm_id(execution.permId);
    exec->set_client_id(static_cast<int32_t>(execution.clientId));
    exec->set_order_id(execution.orderId);
    exec->set_cum_qty(DecimalFunctions::decimalToDouble(execution.cumQty));
    exec->set_avg_price(execution.avgPrice);
    exec->set_order_ref(execution.orderRef);
    exec->set_model_code(execution.modelCode);
    exec->set_last_liquidity(execution.lastLiquidity);
    exec->set_pending_price_revision(execution.pendingPriceRevision);

    m_state_queue.push(msg);
}


// ---- Historical ----
void TradingWrapper::historicalData(TickerId reqId, const Bar& bar) {
    StateMessage msg;
    msg.set_type(STATE_HIST_BAR);
    msg.set_id(static_cast<int64_t>(reqId));
    *msg.mutable_ts() = make_timestamp();

    auto* pb = msg.mutable_hist_bar()->mutable_bar();
    pb->set_time(bar.time);
    pb->set_open(bar.open);
    pb->set_high(bar.high);
    pb->set_low(bar.low);
    pb->set_close(bar.close);
    pb->set_volume(DecimalFunctions::decimalToDouble(bar.volume));
    pb->set_wap(DecimalFunctions::decimalToDouble(bar.wap));
    pb->set_count(bar.count);

    m_state_queue.push(msg);
}

void TradingWrapper::historicalDataEnd(i32 reqId, const std::string& start, const std::string& end) {
    StateMessage msg;
    msg.set_type(STATE_HIST_END);
    msg.set_id(reqId);
    *msg.mutable_ts() = make_timestamp();

    auto* pb = msg.mutable_hist_end();
    pb->set_start(start);
    pb->set_end(end);

    m_state_queue.push(msg);
}


}
