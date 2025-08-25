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

        void init(const fs::path& imageFilepath, TextureFilter filter);
        void init(const TextureSpecs& specs);

        virtual void makeActive()   const override;
        virtual void makeInactive() const override;

        virtual ~OpenGLTexture();
    };

    // Render texture management
    void initRenderTextureOpenGL(RenderTexture2D& texture, int32 width, int32 height);
    void resizeRenderTextureOpenGL(RenderTexture2D& texture, int32 width, int32 height);
    void unloadRenderTextureOpenGL(RenderTexture2D& texture);
} // namespace mirras
