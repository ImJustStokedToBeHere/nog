#include <algorithm>
#include <cstddef>
#include <memory>
#include <utility>
#include <vector>

namespace nog {
    class BumpAllocator {
    private:
        // Represents a continuous chunk of raw memory
        struct Block {
            std::unique_ptr<std::byte[]> memory;
            std::size_t size;
            std::size_t offset = 0;

            Block(std::size_t capacity) : memory(std::make_unique<std::byte[]>(capacity)), size(capacity) {}
        };

        // Erasure type layout to track object destructors
        struct DestructorTask {
            void* object_ptr;
            void (*destructor_fn)(void*);
        };

        std::vector<Block> blocks;
        std::vector<DestructorTask> destructors;
        std::size_t default_block_size;

        // Helper to find or allocate a block with enough space and correct alignment
        void* allocate_raw(std::size_t bytes, std::size_t alignment) {
            if (!blocks.empty()) {
                auto& current = blocks.back();
                void* pointer = current.memory.get() + current.offset;
                std::size_t space = current.size - current.offset;

                if (std::align(alignment, bytes, pointer, space)) {
                    current.offset = static_cast<std::byte*>(pointer) - current.memory.get() + bytes;
                    return pointer;
                }
            }

            // Current block doesn't have enough space or fails alignment; allocate a new block
            std::size_t next_size = std::max(default_block_size, bytes + alignment);
            blocks.push_back(Block(next_size));

            auto& current = blocks.back();
            void* pointer = current.memory.get();
            std::size_t space = current.size;

            std::align(alignment, bytes, pointer, space);
            current.offset = static_cast<std::byte*>(pointer) - current.memory.get() + bytes;
            return pointer;
        }

    public:
        explicit BumpAllocator(std::size_t initial_block_size = 4096) : default_block_size(initial_block_size) {}

        // Delete copy operations to maintain explicit resource ownership
        BumpAllocator(const BumpAllocator&) = delete;
        BumpAllocator& operator=(const BumpAllocator&) = delete;

        // Move operations
        BumpAllocator(BumpAllocator&&) noexcept = default;
        BumpAllocator& operator=(BumpAllocator&&) noexcept = default;

        ~BumpAllocator() { reset(); }

        // Allocate and construct an object of type T
        template <typename T, typename... Args>
        T* emplace(Args&&... args) {
            void* raw_mem = allocate_raw(sizeof(T), alignof(T));
            T* object_ptr = ::new (raw_mem) T(std::forward<Args>(args)...);

            // Register destructor if the type is not trivially destructible
            if constexpr (!std::is_trivially_destructible_v<T>) {
                destructors.push_back({object_ptr, [](void* ptr) { static_cast<T*>(ptr)->~T(); }});
            }
            return object_ptr;
        }

        // Resets the allocator. Invokes all destructors and opens up all buffers for reuse.
        void reset() {
            // Call destructors in reverse order of allocation
            for (auto it = destructors.rbegin(); it != destructors.rend(); ++it) {
                it->destructor_fn(it->object_ptr);
            }
            destructors.clear();

            // Keep the allocated memory blocks, but reset their indices to empty
            for (auto& block : blocks) {
                block.offset = 0;
            }
        }
    };
} // namespace nog
