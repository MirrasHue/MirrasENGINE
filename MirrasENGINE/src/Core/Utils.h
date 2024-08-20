#pragma once

#include <cstdint>
#include <stdexcept>

using int32 = int32_t;
using uint32 = uint32_t;
using uint16 = uint16_t;
using uint8 = uint8_t;

namespace mirras
{
    template <typename T>
    struct Vec2
    {
        T x{}, y{};
    };

    using vec2i = Vec2<int32>;
    using vec2f = Vec2<float>;

    // Used for signaling that something outside our control went wrong,
    // e.g., not being able to initialize some resource required by the engine
    inline void external_adversity(const char* what)
    {
        throw std::runtime_error(what);
    }
}

