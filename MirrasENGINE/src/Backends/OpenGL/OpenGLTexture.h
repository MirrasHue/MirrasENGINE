#pragma once

#include "Core/Renderer/Texture.h"

namespace mirras
{
    class OpenGLTexture : public Texture
    {
    public:
        OpenGLTexture(const fs::path& imageFilepath);

        virtual void makeActive()   const override;
        virtual void makeInactive() const override;
    };
} // namespace mirras
