#pragma once

#include "Core/Types/Integer.h"
#include "Core/Asserts.h"

#include <type_traits>

namespace mirras
{
    class Rand
    {
    public:
        Rand() = delete;

        static uint64 next();
        // Generates a random value in the range [0.0, 1.0)
        static double nextNormalized();

        template<typename T>
        static T range(T min, T max)
        {
            MIRR_ASSERT_RETURN_VALUE(min < max, {}, "Invalid range, Max must be greater than Min");
            
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

        static void seed(uint64 value);
        // Seeds the generator with a random value and returns the seed in case it's needed
        static uint64 seed();

    private:
        // [0, value)
        static uint64 nextUnder(uint64 value);

    private:
        static uint64 state;
    };
} // namespace mirras
