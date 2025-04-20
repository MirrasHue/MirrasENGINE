#pragma once

#include <memory>

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
} // namespace mirras
