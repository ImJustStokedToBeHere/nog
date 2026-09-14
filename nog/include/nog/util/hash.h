
#pragma once
#include <cctype>
#include <cstring>
#include <functional>
#include <span>
#include <string>
#include <string_view>
// #include <sstream>
// #include <algorithm>

namespace nog {
    namespace hash {
        static constexpr size_t COMBINE_CONST{0xA5555529};

        inline static size_t combine(size_t new_key, size_t current_key) {
            return (current_key * COMBINE_CONST) + new_key;
        }

        inline static size_t combine(bool new_part, size_t current_key) {
            return combine(current_key, new_part ? 1 : 0);
        }

        template <typename Type>
        inline size_t combine(const Type* new_part, size_t current_key) {
            size_t tmp{current_key * COMBINE_CONST};
            if (new_part) {
                return tmp + std::hash<Type>(new_part)();
            } else {
                return tmp;
            }
        }

        template <typename Type>
        inline size_t combine(const Type& new_part, size_t current_key) {
            return (current_key * COMBINE_CONST) + std::hash<Type>(new_part);
        }

        inline static constexpr size_t OFFSET_BIAS{2166136261};
        inline static constexpr size_t PRIME{1677619};

        inline static size_t get_hashcode(std::span<unsigned char> data) {
            size_t hc{OFFSET_BIAS};
            for (size_t i = 0; i < data.size(); i++)
                hc = (hc ^ data[i]) * PRIME;

            return hc;
        }

        inline static size_t get_hashcode(std::span<unsigned char> data, bool& is_ascii_data) {
            size_t hc{OFFSET_BIAS};
            char ascii_mask{0};
            for (size_t i = 0; i < data.size(); i++) {
                char byte = data[i];
                ascii_mask |= byte;
                hc = (hc ^ byte) * PRIME;
            }

            is_ascii_data = (ascii_mask & 0x80) == 0;

            return hc;
        }

        inline static size_t get_hashcode(std::string_view txt, size_t start, size_t len) {
            size_t hc{OFFSET_BIAS};
            size_t end = start + len;

            for (size_t i = 0; i < end; i++) {
                hc = (hc ^ txt[i]) * PRIME;
            }

            return hc;
        }

        inline static size_t get_case_insensitive_hashcode(const std::string& txt, size_t start, size_t len) {
            std::string s{};

            for (auto c : txt) {
                s.append(1, std::tolower(c));
            }

            return get_hashcode(s, start, len);
        }

        inline static size_t combine_hashcode(size_t hashcode, unsigned char c) { return (hashcode ^ c) * PRIME; }

        inline static size_t combine_hashcode(size_t hashcode, std::string_view txt) {
            for (auto c : txt) {
                hashcode = (hashcode ^ c) * PRIME;
            }

            return hashcode;
        }

        inline static size_t get_hashcode(unsigned char c) { return combine_hashcode(OFFSET_BIAS, c); }

    } // namespace hash
    //
    inline static void hash_combine(size_t& seed) {}

    template <typename Type, typename... OtherTypes>
    inline static void hash_combine(size_t& seed, const Type& value, OtherTypes... other_values) {
        std::hash<Type> hasher;
        seed ^= hasher(value) + 0x9E3779B9 + (seed << 6) + (seed >> 2);
        hash_combine(seed, other_values...);
    }

} // namespace nog
//
#define MAKE_HASHABLE(type, ...) \
    namespace std { \
        template <> \
        struct hash<type> { \
            size_t operator()(const type& t) const noexcept { \
                size_t ret = 0; \
                nog::hash_combine(ret, t, __VA_ARGS__); \
                return ret; \
            } \
        }; \
    }
