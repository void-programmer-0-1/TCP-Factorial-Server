#pragma once
#include <cstdint>
#include <utility>

// Fast doubling algorithm
// Returns {F(n), F(n+1)}
inline std::pair<uint64_t, uint64_t> fib_pair(uint64_t n) {
    if (n == 0)
        return {0, 1};

    auto [a, b] = fib_pair(n >> 1);

    uint64_t c = a * ((b << 1) - a);
    uint64_t d = a * a + b * b;

    if (n & 1)
        return {d, c + d};
    else
        return {c, d};
}

inline uint64_t fib(uint64_t n) {
    return fib_pair(n).first;
}
