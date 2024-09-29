#include "Backends/OpenGL/OpenGLTexture.h"

#include <raylib/rlgl.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/image.h>

#include "Utilities/FileIO.h"

namespace mirras
{
    OpenGLTexture::OpenGLTexture(const fs::path& imageFilepath)
    {
        std::string imageRawContent = file::read(imageFilepath);

        auto* image = stbi_load_from_memory((const stbi_uc*)imageRawContent.data(), imageRawContent.size(), &width, &height, &channels, 0);

        if(!image)
        {
            ENGINE_LOG_ERROR("Could not process the image read from file: {}", imageFilepath.string());
            return;
        }

        mipmaps = 1; // Default value used by raylib

        if(channels == 1)      format = RL_PIXELFORMAT_UNCOMPRESSED_GRAYSCALE;
        else if(channels == 2) format = RL_PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA;
        else if(channels == 3) format = RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8;
        else if(channels == 4) format = RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

        id = rlLoadTexture(image, width, height, format, mipmaps);

        stbi_image_free(image);
    }

    void OpenGLTexture::makeActive() const
    {
        rlSetTexture(id);
    }

    void OpenGLTexture::makeInactive() const
    {
        rlSetTexture(0);
    }
}