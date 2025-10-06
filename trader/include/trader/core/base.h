#pragma once

#include <cstdint>
#include <string>

namespace trader {

// Signed integer aliases
using i8  = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

// Unsigned integer aliases
using u8  = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

// Floating-point aliases
using f32 = float;
using f64 = double;

using Text = std::string; 


// Fixed buffer sizes
constexpr u64 MSG_LEN     = 256; // error strings, headlines
constexpr u64 STATUS_LEN  = 32;  // order status strings
constexpr u64 SYMBOL_LEN  = 16;  // ticker symbols ("AAPL", "ES", etc.)
constexpr u64 EXCHANGE_LEN = 16; // exchange identifiers ("SMART", "NYSE")
constexpr u64 CURRENCY_LEN = 8;  // currencies ("USD", "EUR")

}
