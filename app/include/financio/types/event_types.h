#pragma once

#include "financio/types/base.h"

enum class TradingEventType : uint8_t {
    Connected,
    Disconnected,
    Error,

    TickPrice,
    TickSize,
    OrderStatus,
    OrderFilled,

    HistoricalBar,
    HistoricalEnd,

    AccountSummary,
    PortfolioUpdate,
    NewsHeadline
};

// TradingEvent: pushed from TradingWorker -> main thread
struct TradingEvent {
    TradingEventType type;
    int32_t id; // tickerId, orderId, reqId

    union {
        // --- Connection / Errors
        struct {
            int32_t code;
            char message[MSG_LEN];
        } error;

        // --- Market data
        struct {
            double price;
            int32_t field;     // BID, ASK, LAST, etc.
            uint32_t attribMask; // TickAttrib encoded
        } tickPrice;

        struct {
            int32_t field;
            double size;
        } tickSize;

        // --- Orders
        struct {
            char status[STATUS_LEN]; // "Submitted", "Filled", etc.
            double filled;
            double remaining;
            double avgFillPrice;
        } orderStatus;

        struct {
            double fillPrice;
            double qty;
        } orderFilled;

        // --- Historical data
        struct {
            double open, high, low, close;
            double volume;
            char date[16]; // e.g. "20250202 09:30"
        } bar;

        struct {
            char start[16];
            char end[16];
        } historicalEnd;

        // --- Account/Portfolio
        struct {
            char account[16];
            char key[32];
            char value[32];
            char currency[8];
        } accountSummary;

        struct {
            char account[16];
            char symbol[SYMBOL_LEN];
            double position;
            double avgCost;
        } portfolio;

        // --- News
        struct {
            char provider[16];
            char headline[MSG_LEN];
        } news;
    };
};
