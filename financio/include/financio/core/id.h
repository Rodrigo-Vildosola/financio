#pragma once
#include <atomic>
#include <cstdint>

namespace app {

// Thread-safe monotonically increasing ID source
inline uint64_t generate_id() {
    static std::atomic<uint64_t> counter{1};
    return counter.fetch_add(1, std::memory_order_relaxed);
}

} // namespace app
