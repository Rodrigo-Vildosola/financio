#include <chrono>
#include <string>

#include <twsapi/Order.h>

#include "financio/client/worker.h"

TradingWorker::TradingWorker() : m_wrapper(m_ev_queue), m_client(&m_wrapper, &m_os_signal) {}

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

    if (m_thread.joinable()) {
        m_thread.join();
    }
}

bool TradingWorker::postRequest(const TradingRequest& req) {
    return m_req_queue.push(req);
}

bool TradingWorker::pollEvent(TradingEvent& ev) {
    return m_ev_queue.pop(ev);
}

void TradingWorker::run() {
    while (m_running) {
        // Handle all queued requests
        TradingRequest req;
        while (m_req_queue.pop(req)) {
            handleRequest(req);
        }

        // Process IB socket messages
        m_os_signal.waitForSignal(); // blocks until socket activity
        if (m_reader) {
            m_reader->processMsgs();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
}

void TradingWorker::handleRequest(const TradingRequest& req) {
    switch (req.type) {
        case TradingRequestType::Connect: {
            const auto& p = std::get<ConnectRequest>(req.payload);
            bool connected = m_client.eConnect(p.host.c_str(), p.port, p.clientId);

            if (connected) {
                m_reader = std::make_unique<EReader>(&m_client, &m_os_signal);
                m_reader->start();
            }
            break;
        }
        case TradingRequestType::Disconnect: {
            m_client.eDisconnect();
            break;
        }
        case TradingRequestType::SubscribeMarketData: {
            const auto& md = std::get<MarketDataRequest>(req.payload);
            Contract c;
            c.symbol   = md.symbol;
            c.secType  = md.secType;
            c.exchange = md.exchange;
            c.currency = md.currency;
            m_client.reqMktData(req.id, c, "", false, false, {});
            break;
        }
        case TradingRequestType::UnsubscribeMarketData: {
            m_client.cancelMktData(req.id);
            break;
        }
        case TradingRequestType::PlaceOrder: {
            const auto& od = std::get<PlaceOrderRequest>(req.payload);
            Contract c;
            c.symbol   = "AAPL";   // TODO: extend payload with contract
            c.secType  = "STK";
            c.exchange = "SMART";
            c.currency = "USD";

            Order o;
            o.action    = od.action;
            o.orderType = od.orderType;
            o.totalQuantity = od.quantity;
            o.lmtPrice  = od.limitPrice;
            o.auxPrice  = od.stopPrice;

            m_client.placeOrder(req.id, c, o);
            break;
        }
        case TradingRequestType::CancelOrder: {
            OrderCancel cancel;
            cancel.manualOrderCancelTime = "";   // default: empty
            cancel.extOperator = "";             // default: empty
            cancel.manualOrderIndicator = UNSET_INTEGER; // default unset

            m_client.cancelOrder(req.id, cancel);
            break;
        }
        case TradingRequestType::RequestHistorical: {
            const auto& hd = std::get<HistoricalRequest>(req.payload);
            Contract c;
            c.symbol   = hd.symbol;
            c.secType  = hd.secType;
            c.exchange = hd.exchange;
            c.currency = hd.currency;

            m_client.reqHistoricalData(
                req.id,
                c,
                "", // end datetime ("" = now)
                hd.durationStr,
                hd.barSize,
                hd.whatToShow,
                hd.useRTH,
                1,  // formatDate = 1 => yyyyMMdd HH:mm:ss
                false,
                {}
            );
            break;
        }
        case TradingRequestType::RequestAccountData: {
            m_client.reqAccountSummary(req.id, "All", "NetLiquidation,BuyingPower");
            break;
        }
        case TradingRequestType::RequestNews: {
            m_client.reqNewsProviders();
            break;
        }
    }
}
