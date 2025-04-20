#pragma once

#include "Core/Types/Integer.h"
#include "Core/Types/Reference.h"
#include "Core/Fwd.h"

#include <filesystem>

namespace mirras
{
    enum class TextureFilter
    {
        Nearest,
        Linear
    };

    struct TextureSpecs
    {
        const uint8* data = nullptr;
        int32 width  = 1;
        int32 height = 1;
        // Number of channels encodes the format
        // 1 - Grayscale no alpha
        // 2 - Grayscale with alpha
        // 3 - RGB  8 bits per component
        // 4 - RGBA 8 bits per component
        int32 channels = 4;
        int32 mipmaps = 1;
        TextureFilter filter = TextureFilter::Linear;
    };

    class Texture
    {
    public:
        virtual void makeActive()   const = 0;
        virtual void makeInactive() const = 0;

        static single_ref<Texture> loadFrom(const fs::path& imageFilepath, TextureFilter filter = TextureFilter::Linear);
        static single_ref<Texture> createFrom(const TextureSpecs& specs);

    public:
        uint32 id{};
        int32 width{}, height{};
        int32 channels{};
        int32 mipmaps{};
    };

    using Texture2D = Texture;
} // namespace mirras
