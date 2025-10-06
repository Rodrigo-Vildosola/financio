#pragma once

#include "trader/core/ring_buffer.h"
#include "trading/control.pb.h"
#include "trading/state.pb.h"

#include "trader/broker/trading_wrapper.h"

// IB API headers
#include <twsapi/EClientSocket.h>
#include <twsapi/EReader.h>
#include <twsapi/Contract.h>

#include <thread>
#include <atomic>
#include <memory>

namespace trader {


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
    bool postRequest(const financio::trading::ControlMessage& c_msq);
    bool pollEvent(financio::trading::StateMessage& s_msg);

private:
    void run();
    void handleRequest(const financio::trading::ControlMessage& c_msq);

private:
    std::atomic<bool> m_running{false};
    std::thread m_thread;

    RingBuffer<financio::trading::ControlMessage, QUEUE_SIZE> m_control_queue;
    RingBuffer<financio::trading::StateMessage, QUEUE_SIZE> m_state_queue;

    TradingWrapper m_wrapper;         // translates IB callbacks -> TradingEvent
    EReaderOSSignal m_os_signal;      // IB signaling
    EClientSocket m_client;           // IB client socket
    std::unique_ptr<EReader> m_reader; // IB reader (must be heap)
};

}
