#pragma once

#include "financio/client/ring_buffer.h"
#include "financio/client/states.h"
#include "financio/client/wrapper.h"

// IB API headers
#include <twsapi/EClientSocket.h>
#include <twsapi/EWrapper.h>
#include <twsapi/EReader.h>
#include <twsapi/Contract.h>

#include <thread>
#include <atomic>
#include <chrono>

class TradingWorker {
public:
    static constexpr size_t QUEUE_SIZE = 1024;

    TradingWorker();
    ~TradingWorker();

    void start();
    void stop();

    bool postRequest(const TradingRequest& req);
    bool pollEvent(TradingEvent& ev);

private:
    void run();
    void handleRequest(const TradingRequest& req);

private:
    std::atomic<bool> m_running;
    std::thread m_thread;

    RingBuffer<TradingRequest, QUEUE_SIZE> m_req_queue;
    RingBuffer<TradingEvent, QUEUE_SIZE> m_ev_queue;

    TradingWrapper m_wrapper;

    EReaderOSSignal m_os_signal;
    EClientSocket m_client;           // avoid heap
    std::unique_ptr<EReader> m_reader; // IB requires dynamic here
};
