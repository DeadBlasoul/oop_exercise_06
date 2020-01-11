#include <iostream>

#include <queue.hpp>
#include <allocator.hpp>

int main() try {
    oop::queue<int, oop::vector_allocator<int, 0x100>> q;
    q.push(1);
    std::cout << q.top() << std::endl;
}
catch (...)
{
    std::cout << "Exception occured" << std::endl;
}
