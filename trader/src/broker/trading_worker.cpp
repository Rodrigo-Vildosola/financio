#include <chrono>
#include <string>

#include <twsapi/Order.h>

#include "trader/broker/trading_worker.h"

#include "trading/control.pb.h"
#include "trading/lifecycle.pb.h"
#include "trading/state.pb.h"

#include <eng/enginio.h>


using namespace financio::trading;

namespace trader {

TradingWorker::TradingWorker() : m_wrapper(m_state_queue), m_client(&m_wrapper, &m_os_signal) {}

TradingWorker::~TradingWorker() {
    stop();
}

void TradingWorker::start() {
    if (m_running) return;
    m_running = true;
    m_thread = std::thread(&TradingWorker::run, this);
}

void TradingWorker::stop() {
    if (!m_running) return;
    m_running = false;

    m_os_signal.issueSignal();

    if (m_thread.joinable()) {
        m_thread.join();
    }
}

bool TradingWorker::postRequest(const ControlMessage& c_msg) {
    const bool ok = m_control_queue.push(c_msg);
    if (ok) m_os_signal.issueSignal();
    return ok;
}

bool TradingWorker::pollEvent(StateMessage& s_msg) {
    return m_state_queue.pop(s_msg);
}

void TradingWorker::run() {
    while (m_running) {
        // Handle all queued requests
        ControlMessage c_msq;
        while (m_control_queue.pop(c_msq)) {
            handleRequest(c_msq);
        }

        // Process IB socket messages
        m_os_signal.waitForSignal(); // blocks until socket activity
        if (m_reader) {
            m_reader->processMsgs();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
}

void TradingWorker::handleRequest(const ControlMessage& cmd) {
    switch (cmd.type()) {

        // --------------------------------------------------
        // Broker connection control (usually in lifecycle)
        // --------------------------------------------------
        case CONTROL_CONNECT: {
            if (cmd.has_connect()) {
                // This assumes ConnectBroker is in lifecycle.proto.
                // You can call this separately if lifecycle messages are routed differently.
                // Example left for completeness.
                ENG_WARN("CONTROL_CONNECT received via TradingCommand; should come from lifecycle instead.");
                break;
            }

            const auto& conn = cmd.connect();
            bool connected = m_client.eConnect(conn.host().c_str(), conn.port(), conn.client_id());

            if (connected) {
                m_reader = std::make_unique<EReader>(&m_client, &m_os_signal);
                m_reader->start();
            }
            break;
        }

        // --------------------------------------------------
        // Broker disconnection control
        // --------------------------------------------------
        case CONTROL_DISCONNECT: {
            m_client.eDisconnect();
            break;
        }

        // --------------------------------------------------
        // Subscribe Market Data
        // --------------------------------------------------
        case CONTROL_SUB_MKT: {
            if (!cmd.has_sub_mkt_data()) {
                ENG_WARN("CONTROL_SUB_MKT missing payload.");
                break;
            }
            const auto& md = cmd.sub_mkt_data();
            Contract c;
            c.symbol   = md.symbol();
            c.secType  = md.sec_type();
            c.exchange = md.exchange();
            c.currency = md.currency();
            m_client.reqMktData(cmd.id(), c, "", false, false, {});
            break;
        }

        // --------------------------------------------------
        // Unsubscribe Market Data
        // --------------------------------------------------
        case CONTROL_UNSUB_MKT: {
            if (!cmd.has_unsub_mkt_data()) {
                ENG_WARN("CONTROL_UNSUBSCRIBE_MARKET_DATA missing payload.");
                break;
            }
            m_client.cancelMktData(cmd.id());
            break;
        }

        // --------------------------------------------------
        // Place Order
        // --------------------------------------------------
        case CONTROL_PLACE_ORDER: {
            if (!cmd.has_place_order()) {
                ENG_WARN("CONTROL_PLACE_ORDER missing payload.");
                break;
            }

            const auto& od = cmd.place_order();
            Contract c;
            c.symbol   = "AAPL";   // TODO: extend payload with contract
            c.secType  = "STK";
            c.exchange = "SMART";
            c.currency = "USD";

            Order o;
            o.action    = od.action();
            o.orderType = od.order_type();
            o.totalQuantity = od.quantity();
            o.lmtPrice  = od.limit_px();
            o.auxPrice  = od.stop_px();

            m_client.placeOrder(cmd.id(), c, o);
            break;
        }

        // --------------------------------------------------
        // Cancel Order
        // --------------------------------------------------
        case CONTROL_CANCEL_ORDER: {
            if (!cmd.has_cancel_order()) {
                ENG_WARN("CONTROL_CANCEL_ORDER missing payload.");
                break;
            }

            const auto& cancel = cmd.cancel_order();
            OrderCancel cancel_struct;
            cancel_struct.manualOrderCancelTime = "";
            cancel_struct.extOperator = "";
            cancel_struct.manualOrderIndicator = UNSET_INTEGER;

            m_client.cancelOrder(cmd.id(), cancel_struct);
            break;
        }

        // --------------------------------------------------
        // Historical Data
        // --------------------------------------------------
        case CONTROL_HISTORICAL: {
            if (!cmd.has_req_historical_data()) {
                ENG_WARN("CONTROL_REQUEST_HISTORICAL missing payload.");
                break;
            }

            const auto& hd = cmd.req_historical_data();
            Contract c;
            c.symbol   = hd.symbol();
            c.secType  = hd.sec_type();
            c.exchange = hd.exchange();
            c.currency = hd.currency();

            m_client.reqHistoricalData(
                cmd.id(),
                c,
                "", // end datetime ("" = now)
                hd.duration_str(),
                hd.bar_size(),
                hd.what_to_show(),
                hd.use_rth(),
                1,  // formatDate = 1 => yyyyMMdd HH:mm:ss
                false,
                {}
            );
            break;
        }

        // --------------------------------------------------
        // Account Summary
        // --------------------------------------------------
        case CONTROL_ACCOUNT_DATA: {
            m_client.reqAccountSummary(cmd.id(), "All", "NetLiquidation,BuyingPower");
            break;
        }

        // --------------------------------------------------
        // News Providers
        // --------------------------------------------------
        case CONTROL_NEWS: {
            m_client.reqNewsProviders();
            break;
        }

        // --------------------------------------------------
        // Default
        // --------------------------------------------------
        default:
            ENG_WARN("Unhandled ControlType: {}", static_cast<int>(cmd.type()));
            break;
    }
}

}
