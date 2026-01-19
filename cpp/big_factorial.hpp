#pragma once

#include <vector>
#include <string>
#include <cstdint>

class BigInt {
    static constexpr uint32_t BASE = 1000000000;
    std::vector<uint32_t> digits; // little endian

public:
    BigInt(uint32_t v = 1) {
        digits.push_back(v);
    }

    void mul(uint32_t m) {
        uint64_t carry = 0;
        for (auto &d : digits) {
            uint64_t cur = carry + uint64_t(d) * m;
            d = uint32_t(cur % BASE);
            carry = cur / BASE;
        }
        while (carry) {
            digits.push_back(uint32_t(carry % BASE));
            carry /= BASE;
        }
    }

    std::string str() const {
        std::string s = std::to_string(digits.back());
        for (int i = digits.size() - 2; i >= 0; --i) {
            std::string part = std::to_string(digits[i]);
            s += std::string(9 - part.size(), '0') + part;
        }
        return s;
    }
};

inline std::string factorial(int n) {
    BigInt r(1);
    for (int i = 2; i <= n; ++i)
        r.mul(i);
    return r.str();
}
