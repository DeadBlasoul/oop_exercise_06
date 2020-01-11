#include <iostream>
#include <string>

#include <point.hpp>
#include <polygon.hpp>
#include <queue.hpp>
#include <allocator.hpp>

using rhombus = basic_polygon<point2d, 4>;

auto constexpr prompt = "~> ";

void read_rhombus(std::istream& in, rhombus& r);

struct print_string_at_loop_end
{
    std::string_view s;

    ~print_string_at_loop_end()
    {
        std::cout << s;
    }
};

int main(const int argc, char* argv[])
{
    oop::queue<rhombus, oop::vector_allocator<rhombus, 0x10>> q;

    std::cout << prompt;
    std::string input;
    while (std::cin >> input)
    {
        print_string_at_loop_end printer{ prompt };

        try {
            if (input == "push")
            {
                rhombus r;
                read_rhombus(std::cin, r);
                q.push(r);
            }
            else if (input == "top")
            {
                rhombus& r = q.top();
                print2d(std::cout, r);
            }
            else if (input == "pop")
            {
                q.pop();
            }
            else if (input == "insert")
            {
                size_t ix;
                rhombus r;
                std::cin >> ix;
                read_rhombus(std::cin, r);

                auto it = q.begin();
                while (ix > 0)
                {
                    ++it;
                    --ix;
                }
                q.insert(it, r);
            }
            else if (input == "erase")
            {
                size_t ix;
                std::cin >> ix;

                auto it = q.begin();
                while (ix > 0)
                {
                    ++it;
                    --ix;
                }
                q.erase(it);
            }
            else if (input == "print")
            {
                size_t i = 0;
                std::for_each(q.begin(), q.end(),
                    [&i](rhombus& r)
                    {
                        std::cout << "[-- " << i << " --]\n\n";
                        print2d(std::cout, r);
                        ++i;
                    }
                );
            }
            else if (input == "less")
            {
                double area;
                std::cin >> area;
                if (area < 0)
                {
                    std::cout << "invalid area" << std::endl;
                    continue;
                }

                for (auto& r : q)
                {
                    if (area2d(r) < area)
                    {
                        print2d(std::cout, r);
                    }
                }
            }
            else if (input == "exit")
            {
                break;
            }
            else
            {
                std::cout << "Unknown command '" << input << "'" << std::endl;
            }
        }
        catch (std::exception & e)
        {
            std::cout << "error: " << e.what() << std::endl;
        }
    }
}

void read_rhombus(std::istream& in, rhombus& r)
{
    auto constexpr precision = 0.000000001L;
    for (auto& p : r)
    {
        in >> p;
    }
    if (in.fail())
    {
        return;
    }

    constexpr size_t size = rhombus::size();
    const double dist = distance(r[0], r[size - 1]);
    for (size_t i = 0; i < size - 1; i++)
    {
        const double next = distance(r[i], r[i + 1]);
        if (std::abs(dist - next) > precision)
        {
            in.setstate(std::ios::failbit);
            break;
        }
    }
}
