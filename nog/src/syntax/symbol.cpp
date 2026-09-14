#include "nog/syntax/symbol.h"

#include <iostream>
#include <limits>

namespace nog {
    Symbol Symbol::ANY{std::numeric_limits<uint32_t>::max() - 2};
    Symbol Symbol::EMPTY{std::numeric_limits<uint32_t>::max() - 1};
    Symbol Symbol::INVALID{std::numeric_limits<uint32_t>::max()};

    char* SymbolCache::CacheInternals::allocate_new_block(size_t size) {
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

    char* SymbolCache::CacheInternals::allocate(size_t size) {
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

    Symbol SymbolCache::CacheInternals::cache_string(std::string_view str) {
        if (this->lookup.size() >= std::numeric_limits<uint32_t>::max()) {
            return Symbol::INVALID;
        }

        if (auto found = this->lookup.find(str); found != this->lookup.end()) {
            return Symbol(found->second);
        }

        char* allocated_mem = allocate(str.size());
        std::memcpy(allocated_mem, str.data(), str.size());
        auto cached_str = std::string_view(allocated_mem, str.size());
        auto idx = static_cast<uint32_t>(this->set.size());

        this->set.push_back(cached_str);
        this->lookup.emplace(cached_str, idx);

        return Symbol(idx);
    }

    std::optional<std::string_view> SymbolCache::CacheInternals::get_cached_string(const Symbol& sym) const {
        std::cout << "if (this->set.size() > sym.index()): " << this->set.size() << ">" << sym.index()
                  << std::endl;

        if (this->set.size() > sym.index())
            return std::optional(this->set[sym.index()]);
        return std::nullopt;
    }

    bool SymbolCache::CacheInternals::str_is_cached(std::string_view str) const {
        return this->lookup.contains(str);
    }

    void SymbolCache::CacheInternals::reset() {
        this->blocks.clear(); // Automatically deletes all unique_ptr blocks
        this->current_block = nullptr;
        this->current_index = 0;
        this->set.clear();
        this->lookup.clear();
    }

    SymbolCache::~SymbolCache() { this->impl.reset(); }

    Symbol SymbolCache::cache_string(std::string_view str) { return this->impl.cache_string(str); }

    std::optional<std::string_view> SymbolCache::get_cached_string(const Symbol& sym) const {
        return this->impl.get_cached_string(sym);
    }

    bool SymbolCache::str_is_cached(std::string_view str) const { return this->impl.str_is_cached(str); }

    void SymbolCache::reset() { this->impl.reset(); }

} // namespace nog
