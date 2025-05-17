#include "Core/Renderer/Texture.h"

#include "Backends/OpenGL/OpenGLTexture.h"

namespace mirras
{
    // Texture
    single_ref<Texture> Texture::loadFrom(const fs::path& imageFilepath, TextureFilter filter)
    {
        return instantiate<OpenGLTexture>(imageFilepath, filter);
    }

    single_ref<Texture> Texture::createFrom(const TextureSpecs& specs)
    {
        return instantiate<OpenGLTexture>(specs);
    }


    // Render texture
    
    // The channels variable of the depth texture is not set, as raylib's reference implementation 
    // uses a format value of 19, which is not even recognized by them or our engine (currently 1 to 4)
    RenderTexture::RenderTexture(int32 width, int32 height)
    {
        // As RenderTexture is not an abstract class, we do this in order to have a nicer API
        initRenderTextureOpenGL(*this, width, height);

        if(initialSize == vec2i{})
            initialSize = {width, height};
    }

    RenderTexture::RenderTexture(RenderTexture&& rhs)
    {
        *this = std::move(rhs);
        initialSize = rhs.initialSize;
    }

    RenderTexture& RenderTexture::operator=(RenderTexture&& rhs)
    {
        id = rhs.id;
        color = std::move(rhs.color);
        depth = std::move(rhs.depth);
        rhs.id = 0; // This avoids deleting a framebuffer that might still be used (when rhs is destroyed)

        return *this;
    }

    void RenderTexture::resize(int32 width, int32 height)
    {
        resizeRenderTextureOpenGL(*this, width, height);
    }

    RenderTexture::~RenderTexture()
    {
        unloadRenderTextureOpenGL(*this);
    }

} // namespace mirras
