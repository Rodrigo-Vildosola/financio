#pragma once

#include "financio/types/base.h"
#include <variant>


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

// ---- Payloads ----

// Error: carries IB error code + message string
struct ErrorData {
    i32 code;      // IB error code
    Text message;  // error message string
};

// TickPrice: streaming price update
struct TickPriceData {
    f64 price;     // price value
    i32 field;     // IB TickType (BID, ASK, LAST, etc.)
    i32 attribMask;// Encoded TickAttrib flags (delayed, pastLimit, preOpen, etc.)
};

// TickSize: streaming size update (bid/ask/last size, volume, etc.)
struct TickSizeData {
    i32 field;     // IB TickType for size (BID_SIZE, ASK_SIZE, etc.)
    f64 size;      // number of shares/contracts
};

// OrderStatus: status changes for a submitted order
struct OrderStatusData {
    Text status;   // "Submitted", "Filled", "Cancelled", etc.
    f64 filled;    // cumulative filled quantity
    f64 remaining; // remaining quantity
    f64 avgFillPrice; // average fill price
};

// OrderFilled: final execution details when an order is completely filled
struct OrderFilledData {
    f64 fillPrice; // price at which it was filled
    f64 qty;       // quantity filled
};

// HistoricalBar: OHLCV bar for historical data requests
struct HistoricalBarData {
    f64 open, high, low, close; // OHLC values
    f64 volume;                 // trade volume
    Text date;                  // bar timestamp, e.g. "20250202 09:30"
};

// HistoricalEnd: marks the end of a historical data stream
struct HistoricalEndData {
    Text start; // start timestamp
    Text end;   // end timestamp
};

// AccountSummary: account-level key/value pairs
struct AccountSummaryData {
    Text account;  // account code
    Text key;      // data key (e.g. "NetLiquidation")
    Text value;    // value (e.g. "12345.67")
    Text currency; // currency code
};

// PortfolioUpdate: position or portfolio line update
struct PortfolioData {
    Text account;   // account code
    Text symbol;    // symbol, e.g. "AAPL"
    f64 position;   // number of shares/contracts
    f64 avgCost;    // average acquisition cost
};

// NewsHeadline: news event pushed by IB API
struct NewsData {
    Text provider;  // news provider code
    Text headline;  // headline text
};

// ---- Variant ----
using TradingPayload = std::variant<
    ErrorData,
    TickPriceData,
    TickSizeData,
    OrderStatusData,
    OrderFilledData,
    HistoricalBarData,
    HistoricalEndData,
    AccountSummaryData,
    PortfolioData,
    NewsData
>;

// ---- Event ----
/// TradingEvent: wrapper around type + id + typed payload.
/// This is pushed from TradingWorker to main thread for processing.
struct TradingEvent {
    TradingEventType type; // discriminator for which payload is active
    i32 id;                // tickerId, orderId, or reqId depending on event
    TradingPayload payload;// actual event data
};
