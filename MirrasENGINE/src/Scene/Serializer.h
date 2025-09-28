#pragma once

#include "Core/Types/Basic.h"

#include <filesystem_fs>

namespace mirras
{
    class Scene;

    bool serialize(Scene& scene, const fs::path& filepath);
    bool deserialize(Scene& scene, const fs::path& filepath, uint32& entityCount);
} // namespace mirras
