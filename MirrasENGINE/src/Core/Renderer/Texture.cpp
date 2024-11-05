#include "Core/Renderer/Texture.h"

#include "Backends/OpenGL/OpenGLTexture.h"

namespace mirras
{
    single_ref<Texture> Texture::loadFrom(const fs::path& imageFilepath, TextureFilter filter)
    {
        return instantiate<OpenGLTexture>(imageFilepath, filter);
    }

    single_ref<Texture> Texture::createFrom(const TextureSpecs& specs)
    {
        return instantiate<OpenGLTexture>(specs);
    }
} // namespace mirras
