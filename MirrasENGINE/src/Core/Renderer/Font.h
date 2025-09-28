#pragma once

#include "Core/Types/Reference.h"
#include "Core/Fwd.h"

#include <filesystem_fs>

namespace msdf_atlas
{
    class FontGeometry;
}

namespace mirras
{
    class Font
    {
    public:
        Font();
        Font(const fs::path& fontFilepath);
        Font(Font&&);
        
        bool loadFrom(const fs::path& fontFilepath);
        
        Font& operator=(Font&&);

        ~Font();

    public:
        single_ref<msdf_atlas::FontGeometry> geometry;
        single_ref<Texture> atlasTexture;
    };
} // namespace mirras
