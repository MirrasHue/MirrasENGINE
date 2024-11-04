#pragma once

#include "Core/Fwd.h"

#include <filesystem>
#include <string>

namespace mirras::file
{
    std::string read(const fs::path& filepath);
    
} // namespace mirras::file
