#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <type_traits>
#include <vector>
#include <stdexcept>

namespace oop
{
    template <typename T, size_t TPoolSize>
    struct vector_allocator
    {
        using value_type = T;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using is_always_equal = std::false_type;

        template <class U, size_t TOtherPoolSize = TPoolSize>
        struct rebind
        {
            using other = vector_allocator<U, TOtherPoolSize>;
        };

    private:
        struct vall_values
        {
            // CONSTRUCTORS:
            /*!
             * @brief common constructor from raw memory pointer
             */
            explicit vall_values(T* container) noexcept
                : mem_start(container)
                  , mem_finish(mem_start)
            {}

            vall_values(const vall_values&)           = delete;
            vall_values(vall_values&& other) noexcept = delete;

            // ASSIGNMENT OPERATORS:
            vall_values& operator=(const vall_values&)           = delete;
            vall_values& operator=(vall_values&& other) noexcept = delete;

            // DESTRUCTOR:
            /*!
             * @brief destructor
             *
             * Verifies that current memory container is not used.
             */
            ~vall_values() noexcept
            {
                assert((free.size() == mem_finish - mem_start) && "memory leak detected");
            }

            // MEMORY POOL METHODS:
            T* reserve_block()
            {
#if !defined(NDEBUG)
                check_memory();
#endif

                // Try reserve block fast
                if ((mem_finish - mem_start) < TPoolSize)
                {
                    T* block = mem_finish;
                    ++mem_finish;
                    return block;
                }

                // Get block from free memory pool
                if (!free.empty())
                {
                    T* block = free.back();
                    free.erase(free.begin() + free.size() - 1);
                    return block;
                }
                throw std::bad_alloc{};
            }

            void free_block(T* block)
            {
                if (block == nullptr)
                {
                    return;
                }

                // Verify block
                if (block < mem_start || mem_finish < block)
                {
                    throw std::runtime_error{"unknown block"};
                }

                // Check UAF behaviour
                if (auto it = std::find(free.begin(), free.end(), block); it != free.end())
                {
                    throw std::runtime_error{"UAF detected"};
                }

                // Push block to the free pool
                free.push_back(block);
            }

        private:
#if !defined(NDEBUG)
            void check_memory() noexcept
            {
                const size_t used_memory_size = mem_finish - mem_start;
                assert(used_memory_size <= TPoolSize && "container of allocator is corrupt");
            }
#endif

        public:
            T*              mem_start;
            T*              mem_finish;
            std::vector<T*> free;
        };

    public:
        vector_allocator()
            : values_(static_cast<T*>(operator new[](sizeof(T) * TPoolSize)))
        {}

        vector_allocator(const vector_allocator&) = delete;
        vector_allocator(vector_allocator&&)      = delete;

        vector_allocator& operator=(const vector_allocator&) = delete;
        vector_allocator& operator=(vector_allocator&&)      = delete;

        ~vector_allocator() noexcept
        {
            operator delete[](values_.mem_start);
        }

        T* allocate(const std::size_t n)
        {
            if (n == 0)
            {
                return nullptr;
            }
            if (n > max_size())
            {
                throw std::invalid_argument{"vector_allocator: bad block size"};
            }
            return values_.reserve_block();
        }

        void deallocate(T* block, const std::size_t n)
        {
            if (n > max_size())
            {
                throw std::invalid_argument{"vector_allocator: bad block size"};
            }
            values_.free_block(block);
        }

        static constexpr size_type max_size()
        {
            return 1;
        }

    private:
        vall_values values_;
    };
}
