#include <map>
#include <list>
#include <utility>

#include <gtest/gtest.h>

#include <allocator.hpp>

auto constexpr pool_size = 0x100;

TEST(STLCONTAINERS, map) {
    std::map<int, int, std::less<>, oop::vector_allocator<std::pair<const int, int>, 100>> map;

    for (size_t i = 0; i < 5; ++i)
    {
        map[i] = i;
    }

    for (size_t i = 0; i < 5; ++i)
    {
        ASSERT_EQ(map[i], i);
    }
}

TEST(STLCONTAINERS, list) {
    std::list<int, oop::vector_allocator<int, pool_size>> list;

    for (size_t i = 0; i < 5; ++i)
    {
        list.push_back(i);
    }

    for (size_t i = 0; i < 5; ++i)
    {
        auto it = list.begin();
        std::advance(it, i);
        ASSERT_EQ(*it, i);
    }
}
