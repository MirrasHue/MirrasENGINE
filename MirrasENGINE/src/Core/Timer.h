#pragma once

#include "Core/Types/Integer.h"

#include <chrono>

namespace mirras
{
    class Timer
    {
    public:
        
        Timer()
        {
            start = std::chrono::steady_clock::now();
        }

        float elapsed()
        {
            auto end = std::chrono::steady_clock::now();
            float seconds = std::chrono::duration<float>(end - start).count();
            start = end;
            return seconds;
        }

    private:
        std::chrono::steady_clock::time_point start;
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
