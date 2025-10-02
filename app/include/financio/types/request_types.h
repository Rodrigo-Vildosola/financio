#pragma once

#include "financio/types/base.h"

// TradingRequestType: operations the main thread can ask the worker to do.
enum class TradingRequestType : u8 {
    Connect,             // Connect to TWS/Gateway
    Disconnect,          // Disconnect
    SubscribeMarketData, // Start streaming quotes
    UnsubscribeMarketData,
    PlaceOrder,          // Submit order
    CancelOrder,         // Cancel order
    RequestHistorical,   // Request historical bars
    RequestAccountData,  // Account/position snapshot
    RequestNews          // Subscribe news feed
};

// TradingRequest: posted by main/UI to the TradingWorker thread.
struct TradingRequest {
    TradingRequestType type;
    i32 id; // tickerId, orderId, reqId

    union {
        // Connection params
        struct {
            char host[64];
            i32 port;
            i32 clientId;
        } connect;

        // Market data
        struct {
            char symbol[SYMBOL_LEN];
            char exchange[EXCHANGE_LEN];
            char currency[CURRENCY_LEN];
            char secType[8]; // "STK", "OPT", "FUT"
        } marketData;

        // Order placement
        struct {
            f64 quantity;
            f64 limitPrice;
            f64 stopPrice;
            char action[4];   // "BUY"/"SELL"
            char orderType[8]; // "MKT", "LMT", "STP"
        } order;

        // Historical data
        struct {
            char symbol[SYMBOL_LEN];
            char exchange[EXCHANGE_LEN];
            char currency[CURRENCY_LEN];
            char secType[8];
            char durationStr[16]; // "1 D", "1 W", "1 Y"
            char barSize[8];      // "1 min", "1 day"
            char whatToShow[16];  // "TRADES", "MIDPOINT"
            i32 useRTH;           // 1 = RTH only
        } historical;
    };
};
