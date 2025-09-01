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
        // If we add another backend in the future, we just do a switch on the available ones
        initRenderTextureOpenGL(*this, width, height);

        if(initialSize == vec2i{})
            initialSize = {width, height};
    }

    RenderTexture::RenderTexture(RenderTexture&& rhs)
    {
        *this = std::move(rhs);
        initialSize = rhs.initialSize;
    }

    void RenderTexture::resize(int32 width, int32 height)
    {
        resizeRenderTextureOpenGL(*this, width, height);
    }

    // We do this so that the initial size is preserved (by intentionally not copying it)
    RenderTexture& RenderTexture::operator=(RenderTexture&& rhs)
    {
        id = rhs.id;
        color = rhs.color;
        depth = rhs.depth;
        width = rhs.width;
        height = rhs.height;
        rhs.id = 0; // This avoids deleting a framebuffer that might still be used (when rhs is destroyed)

        return *this;
    }

    RenderTexture::~RenderTexture()
    {
        unloadRenderTextureOpenGL(*this);
    }

    // Render texture Ex (extra color buffer)

    RenderTextureEx::RenderTextureEx(int32 width, int32 height)
    {
        initRenderTextureExOpenGL(*this, width, height);

        if(initialSize == vec2i{})
            initialSize = {width, height};
    }

    RenderTextureEx::RenderTextureEx(RenderTextureEx&& rhs)
    {
        *this = std::move(rhs);
        initialSize = rhs.initialSize;
    }

    void RenderTextureEx::resize(int32 width, int32 height)
    {
        resizeRenderTextureExOpenGL(*this, width, height);
    }

    void RenderTextureEx::clear(Attachment index, int32 value) const
    {
        clearColorAttachmentOpenGL(index, value);
    }

    int32 RenderTextureEx::readPixel(Attachment index, int32 x, int32 y) const
    {
        return readRenderTexPixelOpenGL(*this, index, x, y);
    }

    RenderTextureEx& RenderTextureEx::operator=(RenderTextureEx&& rhs)
    {
        id = rhs.id;
        color = rhs.color;
        redInt = rhs.redInt;
        depth = rhs.depth;
        width = rhs.width;
        height = rhs.height;
        rhs.id = 0; // This avoids deleting a framebuffer that might still be used (when rhs is destroyed)

        return *this;
    }

    RenderTextureEx::~RenderTextureEx()
    {
        unloadRenderTextureExOpenGL(*this);
    }
} // namespace mirras
