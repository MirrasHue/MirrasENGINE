#pragma once

#include "Core/BasicTypes.h"
#include "Core/Fwd.h"

#include <filesystem>

namespace msdf_atlas
{
    class FontGeometry;
}

namespace mirras
{
    class Font
    {
    public:
        Font(const fs::path& fontFilepath);
        ~Font();

        single_ref<msdf_atlas::FontGeometry> geometry;
        single_ref<Texture> atlasTexture;
    };
} // namespace mirras
