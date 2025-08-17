#pragma once

#include "Core/Types/Integer.h"

#include <chrono>
#include <thread>

namespace mirras
{
    class Timer
    {
    public:
        
        Timer() :
            start{clock::now()} {}

        double elapsed()
        {
            auto end = clock::now();
            auto seconds = std::chrono::duration<double>(end - start).count();
            start = end;
            return seconds;
        }

    private:
        using clock = std::chrono::steady_clock;
        clock::time_point start;
    };

    inline void wait(double seconds)
    {
        auto start = std::chrono::steady_clock::now();
        auto end = start + std::chrono::duration<double>(seconds);

        while(std::chrono::steady_clock::now() < end)
        {
            std::this_thread::yield();
        }
    }

    inline constexpr double operator""_s(long double seconds)
    {
        return seconds;
    }

    inline constexpr double operator""_s(uint64 seconds)
    {
        return seconds;
    }
    
    inline constexpr double operator""_ms(long double seconds)
    {
        return seconds / 1000.0;
    }

    inline constexpr double operator""_ms(uint64 seconds)
    {
        return seconds / 1000.0;
    }

    inline constexpr double operator""_us(long double seconds)
    {
        return seconds / 1e6;
    }

    inline constexpr double operator""_us(uint64 seconds)
    {
        return seconds / 1e6;
    }
} // namespace mirras
