#pragma once

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
        std::chrono::time_point<std::chrono::steady_clock> start;
    };
    
}
