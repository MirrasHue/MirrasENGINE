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

    // Maybe create a new file for types related to graphics, if we see they are not suited to be here
    template<typename T>
    struct Rect
    {
        T x{}, y{}; // Top left corner
        T width{}, height{};

        // We could use the <=> operator, but the default comparisons may not work as intended, because they take into
        // consideration each member separately, and that is problematic when expecting the area to be compared for example

        // Only meant to be used for member wise comparison
        bool operator ==(const Rect<T>& rhs) const = default;
        bool operator !=(const Rect<T>& rhs) const = default;
    };

    // The 4 is redundant, I know, but it looks nicer and avoids using capital letters or _
    using rect4i = Rect<int32>;
    using rect4f = Rect<float>;
}