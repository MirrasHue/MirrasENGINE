#pragma once

#include "Core/Types/Integer.h"

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
        static T range(T min, T max);

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
