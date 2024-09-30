#pragma once

#include <stdexcept>

namespace mirras
{
    // Used for signaling that something outside our control went wrong,
    // e.g., not being able to initialize some resource required by the engine
    inline void external_adversity(const char* what)
    {
        throw std::runtime_error(what);
    }
} // namespace mirras
