#pragma once

#include <memory>

#include "allocator.hpp"

namespace oop
{
    template<typename Q>
    class queue_forward_iterator
    {
        
    };

    template <typename T, typename TBaseAllocator = std::allocator<T>>
    class queue
    {
        /*!
         * @brief struct node declaration
         */
        struct node;

        /*!
         * @brief internal allocator type
         */
        using allocator = typename std::allocator_traits<TBaseAllocator>::template rebind_alloc<node>;

        /*!
         * @brief custom deleter for smart pointers
         */
        struct deleter
        {
            explicit deleter(allocator& al) noexcept
                : al_(al)
            {}

            void operator()(node* ptr)
            {
                std::allocator_traits<allocator>::destroy(al_, ptr);
                al_.deallocate(ptr, 1);
            }

        private:
            allocator& al_;
        };

        /*!
         * @brief node type definition
         */
        struct node
        {
            T                              value;
            std::unique_ptr<node, deleter> next;

            explicit node(const T& v, deleter& d) noexcept
                : value(v)
                , next(nullptr, d)
            {}
        };

    public:
        queue()
            : deleter_(al_)
            , first_(nullptr, deleter_)
            , last_(nullptr)
            , size_(0)
        {}

        void pop()
        {
            if (first_)
            {
                first_ = std::move(first_->next);
            }

            --size_;
        }

        void push(const T& v)
        {
            node* obj = al_.allocate(1);
            std::allocator_traits<allocator>::construct(al_, obj, v, deleter_);
            if (last_)
            {
                last_->next.reset(obj);
            }
            else
            {
                first_.reset(obj);
            }
            last_ = obj;

            ++size_;
        }

        [[nodiscard]] T& top()
        {
            return first_->value;
        }

        [[nodiscard]] size_t size() const noexcept
        {
            return size_;
        }

    private:
        allocator al_;
        deleter   deleter_;

        std::unique_ptr<node, deleter> first_;
        node*                          last_;
        size_t                         size_;

        friend struct node;
    };
}
