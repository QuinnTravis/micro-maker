#pragma once
#include <atomic>
#include <vector>
#include <cassert>

// Single-producer single-consumer lock-free ring buffer for fixed-size T
template<typename T>
class SpscRing {
public:
    explicit SpscRing(size_t capacity) : cap(capacity+1), buf(cap) {
        head.store(0, std::memory_order_relaxed);
        tail.store(0, std::memory_order_relaxed);
    }

    bool push(const T &item) {
        size_t h = head.load(std::memory_order_relaxed);
        size_t next = (h + 1) % cap;
        if (next == tail.load(std::memory_order_acquire)) return false; // full
        buf[h] = item;
        head.store(next, std::memory_order_release);
        return true;
    }

    bool pop(T &out) {
        size_t t = tail.load(std::memory_order_relaxed);
        if (t == head.load(std::memory_order_acquire)) return false; // empty
        out = buf[t];
        tail.store((t + 1) % cap, std::memory_order_release);
        return true;
    }

    bool empty() const {
        return head.load(std::memory_order_acquire) == tail.load(std::memory_order_acquire);
    }

private:
    const size_t cap;
    std::vector<T> buf;
    std::atomic<size_t> head;
    std::atomic<size_t> tail;
};