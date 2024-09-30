#pragma once

#include <filesystem>
#include <string>

#include "Core/Fwd.h"

namespace mirras::file
{
    std::string read(const fs::path& filepath);
    
} // namespace mirras::file
