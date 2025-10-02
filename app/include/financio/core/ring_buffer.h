#pragma once
#include <atomic>
#include <cstddef>

template<typename T, size_t Capacity>
class RingBuffer {
public:
    RingBuffer() : m_head(0), m_tail(0) {}

    bool push(const T& item) {
        size_t head = m_head.load(std::memory_order_relaxed);
        size_t next = (head + 1) % Capacity;
        if (next == m_tail.load(std::memory_order_acquire)) {
            return false; // full
        }
        m_data[head] = item;
        m_head.store(next, std::memory_order_release);
        return true;
    }

    bool pop(T& item) {
        size_t tail = m_tail.load(std::memory_order_relaxed);
        if (tail == m_head.load(std::memory_order_acquire)) {
            return false; // empty
        }
        item = m_data[tail];
        m_tail.store((tail + 1) % Capacity, std::memory_order_release);
        return true;
    }

    bool empty() const {
        return m_head.load() == m_tail.load();
    }

private:
    T m_data[Capacity];
    std::atomic<size_t> m_head;
    std::atomic<size_t> m_tail;
};
