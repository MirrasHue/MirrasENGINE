#pragma once

#include "Core/Renderer/Texture.h"
#include "Core/Types/Reference.h"

#include <filesystem_fs>

namespace mirras
{
    class ContentBrowserPanel
    {
    public:
        ContentBrowserPanel();

        void draw();

    private:
        fs::path baseDirectory;
        fs::path currentDirectory;
        single_ref<Texture> directoryIcon;
        single_ref<Texture> fileIcon;
        float iconSize = 128.f;
        float spacingX = 16.f;
    };
} // namespace mirras
