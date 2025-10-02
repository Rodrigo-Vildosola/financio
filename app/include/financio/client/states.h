#pragma once

enum class TradingRequestType {
    Connect,
    Disconnect,
    SubscribeMarketData,
    PlaceOrder
    // … extend
};

struct TradingRequest {
    TradingRequestType type;
    int id;   // tickerId or orderId
    // simple data, union for payload
};

enum class TradingEventType {
    Connected,
    Disconnected,
    Error,
    TickPrice,
    OrderStatus,
    HistoricalBar,
    // …
};

struct TradingEvent {
    TradingEventType type;
    int id;

    union {
        struct { double price; int field; int attribMask; } tick;
        struct { int code; char message[128]; } error;
        struct { double open, high, low, close; long volume; } bar;
        // … extend
    };
};
