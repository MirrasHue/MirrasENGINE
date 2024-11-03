#include "Backends/OpenGL/OpenGLTexture.h"

#include "Core/Log.h"

#include "Utilities/FileIO.h"

#include <raylib/rlgl.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/image.h>

namespace mirras
{
    static int32 channelsToRaylibFormat(int32 channels)
    {
        int32 format{};

        if(channels == 1)      format = RL_PIXELFORMAT_UNCOMPRESSED_GRAYSCALE;
        else if(channels == 2) format = RL_PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA;
        else if(channels == 3) format = RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8;
        else if(channels == 4) format = RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

        return format;
    }

    OpenGLTexture::OpenGLTexture(const fs::path& imageFilepath)
    {
        auto* image = stbi_load(imageFilepath.string().c_str(), &width, &height, &channels, 0);

        if(!image)
        {
            ENGINE_LOG_ERROR("Could not load image from: {}", imageFilepath.string());
            return;
        }

        mipmaps = 1; // Default value used by raylib

        id = rlLoadTexture(image, width, height, channelsToRaylibFormat(channels), mipmaps);

        if(id == 0)
        {
            ENGINE_LOG_ERROR("Failed to load OpenGL texture");
        }

        stbi_image_free(image);
    }

    OpenGLTexture::OpenGLTexture(const TextureSpecs& specs) 
    {
        width = specs.width;
        height = specs.height;
        channels = specs.channels;
        mipmaps= specs.mipmaps;

        if(!specs.data)
        {
            ENGINE_LOG_WARN("Creating texture with no pixel data");
        }

        id = rlLoadTexture(specs.data, width, height, channelsToRaylibFormat(channels), mipmaps);

        // TODO: add an option to choose the filter in TextureSpecs
        rlTextureParameters(id, RL_TEXTURE_MIN_FILTER, RL_TEXTURE_FILTER_LINEAR);
        rlTextureParameters(id, RL_TEXTURE_MAG_FILTER, RL_TEXTURE_FILTER_LINEAR);

        if(id == 0)
        {
            ENGINE_LOG_ERROR("Failed to load OpenGL texture");
        }
    }

    void OpenGLTexture::makeActive() const
    {
        rlSetTexture(id);
    }

    void OpenGLTexture::makeInactive() const
    {
        rlSetTexture(0);
    }
} // namespace mirras
