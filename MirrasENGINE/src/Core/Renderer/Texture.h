#pragma once

#include "Core/Types/Basic.h"
#include "Core/Types/Integer.h"
#include "Core/Types/Reference.h"
#include "Core/Fwd.h"

#include <filesystem_fs>

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

        virtual ~Texture() = default;

    public:
        uint32 id{};
        int32 width{}, height{};
        int32 channels{};
        int32 mipmaps{};
    };

    class RenderTexture
    {
    public:
        RenderTexture() = default;
        RenderTexture(int32 width, int32 height);

        RenderTexture(RenderTexture&& rhs);
        RenderTexture& operator=(RenderTexture&& rhs);

        void resize(int32 width, int32 height);

        ~RenderTexture();

    public:
        uint32 id{};
        single_ref<Texture> color;
        single_ref<Texture> depth;

        // Used by Camera2D in order to calculate offsets and zoom scale correctly
        // Initialized in the constructor once
        vec2i initialSize;
    };

    using Texture2D = Texture;
    using RenderTexture2D = RenderTexture;
} // namespace mirras
