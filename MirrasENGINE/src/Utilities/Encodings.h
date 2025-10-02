#pragma once

#include "Core/Types/Basic.h"

#include <string>

namespace mirras
{
    std::u32string stringToU32string(const std::string& str);
    std::string u32stringToString(const std::u32string& u32str);
} // namespace mirras
