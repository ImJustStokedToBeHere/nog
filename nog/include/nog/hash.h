#pragma once
#include <functional>

namespace nog {
    template <typename T>
    void combine_hashes(std::size_t& seed, const T& value) {
        std::hash<T> hasher;
        seed ^= hasher(value) + 0xDEADFADE + (seed << 6) + (seed >> 2);
    }
} // namespace nog
