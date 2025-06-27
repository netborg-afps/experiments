#pragma once

#include <chrono>

typedef std::chrono::high_resolution_clock::time_point time_point_t;
typedef std::chrono::microseconds microseconds;

time_point_t now() {
    return std::chrono::high_resolution_clock::now();
}

uint64_t duration( time_point_t t0, time_point_t t1 ) {
    return std::chrono::duration_cast<
        std::chrono::microseconds> (t1 - t0).count();
}