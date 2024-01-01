#pragma once
#include <functional>
#include <execution>
#include <algorithm>
#include <utility>
#include <vector>
#include <numeric>
#include <thread>

#define PARALLEL

class Concurrency final
{
public:
    template <class It, class Fn>
    static void ForEach(It first, It last, Fn func)
    {
#ifdef PARALLEL
        unsigned int numThreads = std::thread::hardware_concurrency();
        std::vector<std::thread> threads(numThreads);

        auto totalElements = std::distance(first, last);
        auto chunkSize = totalElements / numThreads;
        auto remainder = totalElements % numThreads;

        auto begin = first;
        for (auto& t : threads)
        {
            auto end = std::next(begin, chunkSize + (remainder > 0 ? 1 : 0));
            remainder--;

            t = std::thread([=, &func]()
            {
                std::for_each(begin, end, func);
            });

            begin = end;
        }

        for (auto& t : threads)
        {
            if (t.joinable())
                t.join();
        }
#else
        std::for_each(first, last, func);
#endif
    }
};
