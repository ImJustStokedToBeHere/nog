#pragma once
#include "nog/util/allocator.h"

#include <compare>
#include <cstdint>
#include <cstring>
#include <limits>
#include <map>
#include <optional>
#include <string_view>

namespace nog {

    typedef uint32_t SymbolIndex;

    class Symbol {
    private:
        uint32_t idx;

    public:
        static Symbol ANY;
        static Symbol EMPTY;
        static Symbol INVALID;

        Symbol(uint32_t val) : idx{val} {}

        uint32_t index() const { return this->idx; }
        bool is_any() const { return *this == Symbol::ANY; }
        bool is_invalid() const { return *this == Symbol::INVALID; }
        bool is_empty() const { return *this == Symbol::EMPTY; }

        friend bool operator==(const Symbol& lhs, const Symbol& rhs) {
            if (lhs.idx == rhs.idx) {
                return true;
            } else {
                return lhs.idx == Symbol::ANY.idx || rhs.idx == Symbol::ANY;
            }
        }

        friend bool operator!=(const Symbol& lhs, const Symbol& rhs) { return !(lhs == rhs); }

        std::strong_ordering operator<=>(const Symbol& other) const noexcept = default;
    };

    class SymbolCache {
    private:
        struct CacheInternals {
            size_t block_size;
            char* current_block;
            size_t current_index;
            std::vector<std::unique_ptr<char[]>> blocks;
            std::vector<std::string_view> set;
            std::map<std::string_view, size_t> lookup;

            explicit CacheInternals(size_t block_size = 64 * 1024)
                : block_size(block_size), current_block(nullptr), current_index(0), set{}, lookup{} {}

            // Disable copying to prevent double-free issues
            CacheInternals(const CacheInternals&) = delete;
            CacheInternals& operator=(const CacheInternals&) = delete;

            ~CacheInternals() { this->reset(); }

            char* allocate_new_block(size_t size);

            // Allocate a raw chunk of bytes
            char* allocate(size_t size);

            Symbol cache_string(std::string_view str);

            std::optional<std::string_view> get_cached_string(const Symbol& sym) const;

            template <size_t LEN>
            Symbol cache_bytes(const std::array<std::byte, LEN>& bytes) const {
                return this->cache_string(bytes.data());
            }

            bool str_is_cached(std::string_view str) const;

            // Free all allocated blocks at once
            void reset();
        };

        CacheInternals impl;

    public:
        // Initialize with a default block size (e.g., 64 KB)
        explicit SymbolCache(size_t block_size = 64 * 1024) : impl{block_size} {}

        // Disable copying to prevent double-free issues
        SymbolCache(const SymbolCache&) = delete;
        SymbolCache& operator=(const SymbolCache&) = delete;

        ~SymbolCache();

        // Cache a string and return a lightweight string_view pointing to the arena
        Symbol cache_string(std::string_view str);

        std::optional<std::string_view> get_cached_string(const Symbol& sym) const;

        bool str_is_cached(std::string_view str) const;

        // Free all allocated blocks at once
        void reset();
    };

} // namespace nog
