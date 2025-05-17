#pragma once

#include "Core/Types/Integer.h"

namespace mirras
{
    template <typename T>
    struct Vec2
    {
        T x{}, y{};

        bool operator ==(const Vec2<T>& rhs) const = default;
    };

    using vec2i = Vec2<int32>;
    using vec2f = Vec2<float>;

    // Maybe create a new file for types related to graphics, if we see they are not suited to be here
    template<typename T>
    struct Rect
    {
        Rect() = default;
        Rect(T _x, T _y, T _width, T _height) :
            x{_x}, y{_y}, width{_width}, height{_height} {}

        // We could use the <=> operator, but the default comparisons may not work as intended, because they take into
        // consideration each member separately, and that is problematic when expecting the area to be compared for example

        // Only meant to be used for member wise comparison
        bool operator ==(const Rect<T>& rhs) const = default;
        bool operator !=(const Rect<T>& rhs) const = default;

        T x{}, y{}; // Top left corner
        T width{}, height{};
    };

    // The 4 is redundant, I know, but it looks nicer and avoids using capital letters or _
    using rect4i = Rect<int32>;
    using rect4f = Rect<float>;
} // namespace mirras
