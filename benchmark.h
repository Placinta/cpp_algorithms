//
// Created by Placinta on 5/5/15.
//

#ifndef ALGS_BENCHMARK_H
#define ALGS_BENCHMARK_H

#include <chrono>

template<typename TimeT = std::chrono::milliseconds>
struct measure
{
    template<typename F, typename ...Args>
    static typename TimeT::rep execution(F func, Args&&... args)
    {
        auto start = std::chrono::system_clock::now();
        func(std::forward<Args>(args)...);
        auto duration = std::chrono::duration_cast< TimeT>
                (std::chrono::system_clock::now() - start);
        return duration.count();
    }
};

#endif //ALGS_BENCHMARK_H
