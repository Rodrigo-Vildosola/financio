#pragma once

#include "trader/core/base.h"
#include <variant>

/// Request types: operations the main thread can ask the TradingWorker to perform.
enum class TradingRequestType : u8 {
    Connect,             // Connect to TWS/Gateway
    Disconnect,          // Disconnect session
    SubscribeMarketData, // Start streaming quotes
    UnsubscribeMarketData,// Stop streaming quotes
    PlaceOrder,          // Submit a new order
    CancelOrder,         // Cancel existing order
    RequestHistorical,   // Request historical OHLCV bars
    RequestAccountData,  // Request account/positions snapshot
    RequestNews          // Subscribe to news feed
};

/// --- Payloads ---

// Connect to IB TWS/Gateway
struct ConnectRequest {
    Text host;   // hostname or IP
    i32 port;    // port (default 7497 for TWS)
    i32 clientId;// client identifier
};

// Market data subscription
struct MarketDataRequest {
    Text symbol;     // e.g. "AAPL"
    Text exchange;   // e.g. "SMART"
    Text currency;   // e.g. "USD"
    Text secType;    // "STK", "OPT", "FUT"
};

// Order placement
struct PlaceOrderRequest {
    f64 quantity;     // number of shares/contracts
    f64 limitPrice;   // limit price (0 if not applicable)
    f64 stopPrice;    // stop price (0 if not applicable)
    Text action;      // "BUY" or "SELL"
    Text orderType;   // "MKT", "LMT", "STP"
};

// Historical data request
struct HistoricalRequest {
    Text symbol;      // e.g. "AAPL"
    Text exchange;    // e.g. "SMART"
    Text currency;    // e.g. "USD"
    Text secType;     // "STK", "FUT", etc.
    Text durationStr; // IB duration string, e.g. "1 D", "1 W"
    Text barSize;     // e.g. "1 min", "1 day"
    Text whatToShow;  // "TRADES", "MIDPOINT", etc.
    i32 useRTH;       // 1 = RTH only, 0 = all hours
};

// No payload needed for: Disconnect, UnsubscribeMarketData, CancelOrder, RequestAccountData, RequestNews
struct EmptyRequest {};

/// --- Variant type for payload ---
using TradingRequestPayload = std::variant<
    EmptyRequest,
    ConnectRequest,
    MarketDataRequest,
    PlaceOrderRequest,
    HistoricalRequest
>;


/// TradingRequest: posted by main/UI to the TradingWorker thread.
/// Contains a type discriminator, an identifier, and a typed payload.
struct TradingRequest {
    TradingRequestType type; // what operation to perform
    i32 id;                  // tickerId, orderId, reqId depending on type
    TradingRequestPayload payload;
};
