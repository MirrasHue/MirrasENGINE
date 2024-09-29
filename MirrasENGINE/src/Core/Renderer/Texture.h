#pragma once

#include <filesystem>

#include "Core/BasicTypes.h"

namespace mirras
{
    namespace fs = std::filesystem;

    class Texture
    {
    public:
        virtual void makeActive()   const = 0;
        virtual void makeInactive() const = 0;

        static single_ref<Texture> loadFrom(const fs::path& imageFilepath);

    public:
        uint32 id{};
        int32 width{}, height{};
        int32 channels{};
        int32 format{};
        int32 mipmaps{};
    };

    using Texture2D = Texture;
}