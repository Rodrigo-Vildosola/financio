#pragma once

#include "financio/types/base.h"

// TradingRequestType: operations the main thread can ask the worker to do.
enum class TradingRequestType : uint8_t {
    Connect,             // Connect to TWS/Gateway
    Disconnect,          // Disconnect
    SubscribeMarketData, // Subscribe to streaming quotes
    UnsubscribeMarketData,
    PlaceOrder,          // Submit an order
    CancelOrder,         // Cancel existing order
    RequestHistorical,   // Request historical bars
    RequestAccountData,  // Positions, balances
    RequestNews          // News subscriptions
};

// TradingRequest: posted by main/UI to the TradingWorker thread.
struct TradingRequest {
    TradingRequestType type;
    int32_t id; // tickerId, orderId, reqId depending on request

    union {
        // --- Connect
        struct {
            char host[64];
            int32_t port;
            int32_t clientId;
        } connect;

        // --- Market data subscription
        struct {
            char symbol[SYMBOL_LEN];   // e.g. "AAPL"
            char exchange[EXCHANGE_LEN]; // e.g. "SMART"
            char currency[CURRENCY_LEN]; // e.g. "USD"
            char secType[8];           // e.g. "STK", "OPT", "FUT"
        } marketData;

        // --- Place order
        struct {
            double quantity;
            double limitPrice;
            double stopPrice;
            char action[4]; // "BUY"/"SELL"
            char orderType[8]; // "MKT", "LMT", "STP"
        } order;

        // --- Historical data request
        struct {
            char symbol[SYMBOL_LEN];
            char exchange[EXCHANGE_LEN];
            char currency[CURRENCY_LEN];
            char secType[8];

            char durationStr[16];  // "1 D", "1 W", etc.
            char barSize[8];       // "1 min", "1 hour"
            char whatToShow[16];   // "TRADES", "MIDPOINT", etc.
            int useRTH;            // Regular trading hours only
        } historical;
    };
};
