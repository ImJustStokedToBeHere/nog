#pragma once
#include "nog/util/allocator.h"

#include <compare>
#include <cstdint>
#include <cstring>
#include <string>
#include <string_view>
#include <unordered_set>

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
        size_t block_size;
        char* current_block;
        size_t current_index;
        std::vector<std::unique_ptr<char[]>> blocks;
        std::unordered_set<std::string_view> set;

        char* allocate_new_block(size_t size) {
            auto block = std::make_unique<char[]>(size);
            char* raw_ptr = block.get();
            blocks.push_back(std::move(block));

            // Only track this as the active block if it's the standard size
            if (size == block_size) {
                current_block = raw_ptr;
                current_index = 0;
            }
            return raw_ptr;
        }

        // Allocate a raw chunk of bytes
        char* allocate(size_t size) {
            if (size == 0)
                return nullptr;

            // If the request exceeds the current block size, allocate a custom oversized block
            if (size > block_size) {
                return allocate_new_block(size);
            }

            // If no block exists or the current one is full, allocate a standard block
            if (!current_block || current_index + size > block_size) {
                allocate_new_block(block_size);
            }

            char* ptr = current_block + current_index;
            current_index += size;
            return ptr;
        }

    public:
        // Initialize with a default block size (e.g., 64 KB)
        explicit SymbolCache(size_t block_size = 64 * 1024)
            : block_size(block_size), current_block(nullptr), current_index(0) {}

        // Disable copying to prevent double-free issues
        SymbolCache(const SymbolCache&) = delete;
        SymbolCache& operator=(const SymbolCache&) = delete;

        // Cache a string and return a lightweight string_view pointing to the arena
        Symbol cache_string(std::string_view str) {
            if (this->set.size() >= std::numeric_limits<uint32_t>::max()) {
                return Symbol::INVALID;
            }

            if (auto found = this->set.find(str); found != this->set.end()) {
                uint32_t dist = std::distance(this->set.begin(), found);
                return Symbol(dist);
            }

            char* allocated_mem = allocate(str.size());
            std::memcpy(allocated_mem, str.data(), str.size());
            auto cached_str = std::string_view(allocated_mem, str.size());
            auto result = this->set.insert(cached_str);
            uint32_t dist = std::distance(this->set.begin(), result.first);

            return Symbol(dist);
        }

        // Free all allocated blocks at once
        void reset() {
            blocks.clear(); // Automatically deletes all unique_ptr blocks
            current_block = nullptr;
            current_index = 0;
        }
    };

} // namespace nog
