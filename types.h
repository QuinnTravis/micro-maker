#pragma once
#include <cstdint>
#include <chrono>

struct Tick {
    int64_t ts_ns;   // nanoseconds epoch from producer
    double price;
    double size;
    int side; // 0=bid tick, 1=ask tick (simplified)
};

struct Order {
    int64_t id;
    double price;
    double size;
    int side; // 0=buy,1=sell
};