#pragma once

#include "financio/types/base.h"

enum class TradingEventType : uint8_t {
    Connected,      // Successfully connected
    Disconnected,   // Lost connection
    Error,          // API or network error

    TickPrice,      // Streaming price update
    TickSize,       // Streaming size update
    OrderStatus,    // Order status update
    OrderFilled,    // Final fill confirmation

    HistoricalBar,  // One historical bar
    HistoricalEnd,  // End of historical response

    AccountSummary, // Key/value account data
    PortfolioUpdate,// Position update
    NewsHeadline    // News event
};

// TradingEvent: pushed from TradingWorker -> main thread
struct TradingEvent {
    TradingEventType type;
    i32 id; // tickerId, orderId, reqId depending on type

    union {
        // Error
        struct {
            i32 code;
            char message[MSG_LEN];
        } error;

        // Tick price
        struct {
            f64 price;
            i32 field;       // IB TickType (BID, ASK, LAST, etc.)
            u32 attribMask;  // Encoded TickAttrib flags
        } tickPrice;

        // Tick size
        struct {
            i32 field;
            f64 size;
        } tickSize;

        // Order status
        struct {
            char status[STATUS_LEN]; // "Submitted", "Filled", etc.
            f64 filled;
            f64 remaining;
            f64 avgFillPrice;
        } orderStatus;

        // Filled execution
        struct {
            f64 fillPrice;
            f64 qty;
        } orderFilled;

        // Historical bar
        struct {
            f64 open, high, low, close;
            f64 volume;
            char date[16]; // e.g. "20250202 09:30"
        } bar;

        // Historical complete
        struct {
            char start[16];
            char end[16];
        } historicalEnd;

        // Account summary
        struct {
            char account[16];
            char key[32];
            char value[32];
            char currency[8];
        } accountSummary;

        // Portfolio update
        struct {
            char account[16];
            char symbol[SYMBOL_LEN];
            f64 position;
            f64 avgCost;
        } portfolio;

        // News headline
        struct {
            char provider[16];
            char headline[MSG_LEN];
        } news;
    };
};
