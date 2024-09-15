#pragma once

#include <cstdint>
#include <memory>

using int64 = int64_t;
using int32 = int32_t;
using int16 = int16_t;
using int8  = int8_t;

using uint64 = uint64_t;
using uint32 = uint32_t;
using uint16 = uint16_t;
using uint8  = uint8_t;

namespace mirras
{
    // Represents a reference that is the single owner of an object
    template<typename T>
    using single_ref = std::unique_ptr<T>;

    template<typename T, typename... Args>
    constexpr single_ref<T> instantiate(Args&&... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    using shared_ref = std::shared_ptr<T>;

    template<typename T, typename... Args>
    constexpr shared_ref<T> instantiate_shared(Args&&... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    template <typename T>
    struct Vec2
    {
        T x{}, y{};
    };

    using vec2i = Vec2<int32>;
    using vec2f = Vec2<float>;
}