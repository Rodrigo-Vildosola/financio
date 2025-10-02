#pragma once

#include "financio/core/ring_buffer.h"
#include "financio/types/request_types.h"
#include "financio/types/event_types.h"
#include "financio/client/trading_wrapper.h"

// IB API headers
#include <twsapi/EClientSocket.h>
#include <twsapi/EReader.h>
#include <twsapi/Contract.h>

#include <thread>
#include <atomic>
#include <memory>

/**
 * TradingWorker
 * -------------
 * Runs the Interactive Brokers client in a dedicated worker thread.
 * Consumes TradingRequest objects from the main thread and produces TradingEvent objects.
 */
class TradingWorker {
public:
    static constexpr size_t QUEUE_SIZE = 1024;

    TradingWorker();
    ~TradingWorker();

    // Lifecycle
    void start();
    void stop();

    // API for main thread
    bool postRequest(const TradingRequest& req);
    bool pollEvent(TradingEvent& ev);

private:
    void run();
    void handleRequest(const TradingRequest& req);

private:
    std::atomic<bool> m_running{false};
    std::thread m_thread;

    RingBuffer<TradingRequest, QUEUE_SIZE> m_req_queue;
    RingBuffer<TradingEvent, QUEUE_SIZE> m_ev_queue;

    TradingWrapper m_wrapper;         // translates IB callbacks -> TradingEvent
    EReaderOSSignal m_os_signal;      // IB signaling
    EClientSocket m_client;           // IB client socket
    std::unique_ptr<EReader> m_reader; // IB reader (must be heap)
};
