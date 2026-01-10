#pragma once
#include <vector>
#include <string>
#include <cstdint>

class BigInt {
    static constexpr uint32_t BASE = 1000000000; // 1e9
    std::vector<uint32_t> a; // little-endian

public:
    BigInt(uint32_t v = 1) {
        a.push_back(v);
    }

    void multiply(uint32_t m) {
        uint64_t carry = 0;
        for (size_t i = 0; i < a.size(); ++i) {
            uint64_t cur = carry + uint64_t(a[i]) * m;
            a[i] = uint32_t(cur % BASE);
            carry = cur / BASE;
        }
        while (carry) {
            a.push_back(uint32_t(carry % BASE));
            carry /= BASE;
        }
    }

    std::string toString() const {
        std::string s = std::to_string(a.back());
        for (int i = (int)a.size() - 2; i >= 0; --i) {
            std::string part = std::to_string(a[i]);
            s += std::string(9 - part.length(), '0') + part;
        }
        return s;
    }
};

inline std::string factorial_cpp(int n) {
    BigInt result(1);
    for (int i = 2; i <= n; ++i)
        result.multiply(i);
    return result.toString();
}
