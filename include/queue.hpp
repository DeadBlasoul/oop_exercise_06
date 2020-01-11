#pragma once

#include <memory>
#include <iterator>

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
        struct forward_iterator
        {
            using value_type        = T;
            using reference         = T&;
            using pointer           = T*;
            using difference_type   = ptrdiff_t;
            using iterator_category = std::forward_iterator_tag;
        
        private:
            using internal_value_type = std::unique_ptr<node, deleter>*;

            forward_iterator(internal_value_type ptr)
                : node_(ptr)
            {}

        public:
            T& operator*() const noexcept
            {
                return (*node_)->value;
            }

            forward_iterator& operator++()
            {
                if (node_->get() == nullptr)
                {
                    throw std::out_of_range{"iterator is out of range"};
                }
                node_ = &(*node_)->next;
                return *this;
            }

            forward_iterator operator++(int)
            {
                forward_iterator it = node_;
                ++(*this);
                return it;
            }

            bool operator==(const forward_iterator& other)
            {
                return node_ == other.node_;
            }

            bool operator!=(const forward_iterator& other)
            {
                return !(*this == other);
            }

        private:
            internal_value_type node_;

            friend queue;
        };

        queue()
            : deleter_(al_)
            , first_(nullptr, deleter_)
            , last_(nullptr)
            , size_(0)
        {}

        void pop()
        {
            if (first_.get() == nullptr)
            {
                throw std::out_of_range("queue is empty");
            }
            
            auto free = first_->next.release();
            first_.reset(free);
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
            if (first_.get() == nullptr)
            {
                throw std::out_of_range("queue is empty");
            }
            return first_->value;
        }

        [[nodiscard]] size_t size() const noexcept
        {
            return size_;
        }

        forward_iterator begin() noexcept
        {
            return &first_;
        }

        forward_iterator end() noexcept
        {
            auto it = begin();
            while (it.node_->get() != nullptr)
            {
                ++it;
            }
            return it;
        }

        void insert(forward_iterator it, const T& v)
        {
            auto free = it.node_->release();
            auto obj  = al_.allocate(1);
            std::allocator_traits<allocator>::construct(al_, obj, v, deleter_);

            it.node_->reset(obj);
            it.node_->get()->next.reset(free);
        }

        void erase(forward_iterator it)
        {
            if (it.node_->get() == nullptr)
            {
                throw std::out_of_range{ "erase iterator is out of range" };
            }
            auto free = it.node_->get()->next.release();
            it.node_->reset(free);
        }

    private:
        allocator al_;
        deleter   deleter_;

        std::unique_ptr<node, deleter> first_;
        node*                          last_;
        size_t                         size_;

        friend node;
        friend forward_iterator;
    };
}
