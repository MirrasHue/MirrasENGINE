#pragma once

#include "Core/Types/Basic.h"

#include <string>

namespace mirras::utf
{
    inline std::string toString(const std::u8string& u8str)
    {
        return std::string{u8str.begin(), u8str.end()};
    }

    std::u32string toU32string(const std::string& str);
    std::string toString(const std::u32string& u32str);
} // namespace mirras
