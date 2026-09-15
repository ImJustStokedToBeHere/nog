#pragma once

#include "nog/util/types.h"

#include <cstdint>
#include <cstring>

namespace nog {
    template <IsIntegerType T>
    T read_at_offset(const void* base_address, std::ptrdiff_t offset) {
        T result;
        const char* ptr = static_cast<const char*>(base_address);
        std::memcpy(&result, ptr + offset, sizeof(T));
        return result;
    }

    union IntConverter {
        uint64_t u64;
        uint32_t u32;
        uint16_t u16;
        uint8_t u8;

        struct {
            uint32_t two_parts1;
            uint32_t two_parts2;
        };

        struct {
            uint16_t four_parts1;
            uint16_t four_parts2;
            uint16_t four_parts3;
            uint16_t four_parts4;
        };

        struct {
            uint8_t eight_parts1;
            uint8_t eight_parts2;
            uint8_t eight_parts3;
            uint8_t eight_parts4;
            uint8_t eight_parts5;
            uint8_t eight_parts6;
            uint8_t eight_parts7;
            uint8_t eight_parts8;
        };
    };

} // namespace nog
