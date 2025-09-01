#pragma once

#include "Core/Renderer/Texture.h"

namespace mirras
{
    class OpenGLTexture : public Texture
    {
    public:
        OpenGLTexture() = default;
        OpenGLTexture(const fs::path& imageFilepath, TextureFilter filter);
        OpenGLTexture(const TextureSpecs& specs);

        virtual void makeActive()   const override;
        virtual void makeInactive() const override;

        virtual ~OpenGLTexture();
    };

    // Render texture management
    void initRenderTextureOpenGL(RenderTexture& texture, int32 width, int32 height);
    void resizeRenderTextureOpenGL(RenderTexture& texture, int32 width, int32 height);
    void unloadRenderTextureOpenGL(RenderTexture& texture);

    // Render texture Ex
    void initRenderTextureExOpenGL(RenderTextureEx& texture, int32 width, int32 height);
    void resizeRenderTextureExOpenGL(RenderTextureEx& texture, int32 width, int32 height);
    void clearColorAttachmentOpenGL(Attachment index, int32 value);
    int32 readRenderTexPixelOpenGL(const RenderTextureEx& texture, Attachment index, int32 x, int32 y);
    void unloadRenderTextureExOpenGL(RenderTextureEx& texture);
} // namespace mirras
