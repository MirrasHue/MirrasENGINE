#include "Utilities/Random.h"

#include "Core/Asserts.h"

#include <random>
#include <chrono>
#include <type_traits>

namespace mirras
{
    static std::random_device rd;
    uint64 Rand::state = Rand::seed();

    // PCG(Permuted Congruential Generator) - RXS_M_XS
    // https://www.pcg-random.org/download.html
    uint64 Rand::next()
    {
        state = state * 6364136223846793005ull + 1442695040888963407ull;
        uint64 xorshifted = ((state >> ((state >> 59u) + 5u)) ^ state) * 12605985483714917081ull;
        return (xorshifted >> 43u) ^ xorshifted;
    }

    double Rand::nextNormalized()
    {
        // Take the top 53 bits to use as the mantissa
        return (next() >> 11) * (1.0 / (1ull << 53));  // Normalize to [0.0, 1.0)
    }

    template<typename T>
    T Rand::range(T min, T max)
    {
        MIRR_ASSERT(min < max, "Invalid range, Max must be greater than Min");
        
        if constexpr(std::is_integral_v<T>)
        {
            uint64 rangeSize = uint64(max) - uint64(min) + 1;
            return min + T(nextUnder(rangeSize));
        }
        else
        if constexpr(std::is_floating_point_v<T>)
        {
            return min + (max - min) * T(nextNormalized());
        }
        else
            static_assert(false, "Error, not a number");
    }  

    void Rand::seed(uint64 value)
    {
        MIRR_ASSERT(value != 0, "Seed cannot be zero");

        state = value;
    }
    
    uint64 Rand::seed()
    {
        uint64 time = std::chrono::steady_clock::now().time_since_epoch().count();

        // Use only the lower 32 bits of the time, as they change more frequently
        return state = (time << 32) | rd();
    }

    // Java's unbiased method to generate a number in the range [0, value)
    // https://www.pcg-random.org/posts/bounded-rands.html
    // I didn't use Lemire's method because it requires 128-bit integers (not standard) for 64-bit output
    uint64 Rand::nextUnder(uint64 value)
    {
        uint64 rand{}, remainder{};

        do {
            rand = next();
            remainder = rand % value;
        }
        while(rand - remainder > (-value));

        return remainder;
    }  
} // namespace mirras
