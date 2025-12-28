#include "Backends/OpenGL/OpenGLTexture.h"

#include "Core/Log.h"
#include "Core/Asserts.h"

#include "Utilities/FileIO.h"
#include "Utilities/Encodings.h"

#include <glad/glad.h>
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

    void OpenGLTexture::applyFilter(TextureFilter filter) const
    {
        switch(filter)
        {
            case TextureFilter::Nearest:
                rlTextureParameters(id, RL_TEXTURE_MIN_FILTER, RL_TEXTURE_FILTER_NEAREST);
                rlTextureParameters(id, RL_TEXTURE_MAG_FILTER, RL_TEXTURE_FILTER_NEAREST);
                break;

            case TextureFilter::Linear:
                rlTextureParameters(id, RL_TEXTURE_MIN_FILTER, RL_TEXTURE_FILTER_LINEAR);
                rlTextureParameters(id, RL_TEXTURE_MAG_FILTER, RL_TEXTURE_FILTER_LINEAR);
                break;
        }
    }

    OpenGLTexture::OpenGLTexture(const fs::path& imageFilepath, TextureFilter filter)
    {
        auto* image = stbi_load(utf::toString(imageFilepath.u8string()).c_str(), &width, &height, &channels, 0);

        if(!image)
        {
            ENGINE_LOG_ERROR("Could not load image from: {}", imageFilepath.string());
            width = height = channels = 0;

            return;
        }

        mipmaps = 1; // Default value used by raylib

        id = rlLoadTexture(image, width, height, channelsToRaylibFormat(channels), mipmaps);

        stbi_image_free(image);

        if(id == 0)
        {
            ENGINE_LOG_ERROR("Failed to load OpenGL texture");
            width = height = channels = mipmaps = 0;

            return;
        }

        applyFilter(filter);
    }

    OpenGLTexture::OpenGLTexture(const TextureSpecs& specs) 
    {
        if(!specs.data)
        {
            ENGINE_LOG_WARN("Creating texture with no pixel data");
        }

        id = rlLoadTexture(specs.data, specs.width, specs.height, channelsToRaylibFormat(specs.channels), specs.mipmaps);

        if(id == 0)
        {
            ENGINE_LOG_ERROR("Failed to load OpenGL texture");
            return;
        }

        width = specs.width;
        height = specs.height;
        channels = specs.channels;
        mipmaps = specs.mipmaps;

        applyFilter(specs.filter);
    }

    void OpenGLTexture::makeActive() const
    {
        rlSetTexture(id);
    }

    void OpenGLTexture::makeInactive() const
    {
        rlSetTexture(0);
    }

    OpenGLTexture::~OpenGLTexture()
    {
        if(id > 0)
            rlUnloadTexture(id);
    }


    // Render texture

    static void initFramebuffer(RenderTexture& texture, int32 width, int32 height)
    {
        MIRR_ASSERT_CORE_RETURN(width > 0 && height > 0, "Invalid render texture size: {} x {}", width, height);

        texture.id = rlLoadFramebuffer();

        if(texture.id == 0)
        {
            ENGINE_LOG_ERROR("Unable to create framebuffer for the render texture");
            return;
        }

        rlEnableFramebuffer(texture.id);

        // Create color texture
        texture.color = rlLoadTexture(nullptr, width, height, RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);

        // Create depth texture
        texture.depth = rlLoadTextureDepth(width, height, false);

        texture.width = width;
        texture.height = height;

        // rlDisableFramebuffer() is called by the caller
    }

    void initRenderTextureOpenGL(RenderTexture& texture, int32 width, int32 height)
    {
        initFramebuffer(texture, width, height);

        // Attach color and depth textures to FBO
        rlFramebufferAttach(texture.id, texture.color, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_TEXTURE2D, 0);
        rlFramebufferAttach(texture.id, texture.depth, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_TEXTURE2D, 0);

        // Check if FBO is valid
        if(!rlFramebufferComplete(texture.id))
        {
            ENGINE_LOG_ERROR("Render texture framebuffer is incomplete. Color tex ID: {}, Depth tex ID: {}",
                texture.color, texture.depth);

            rlUnloadTexture(texture.color);
            rlUnloadTexture(texture.depth);
            rlUnloadFramebuffer(texture.id);
            texture.id = texture.color = texture.depth = 0;
        }

        rlDisableFramebuffer();
    }

    void resizeRenderTextureOpenGL(RenderTexture& texture, int32 width, int32 height)
    {
        MIRR_ASSERT_CORE_RETURN(texture.id > 0 && width > 0 && height > 0,
            "Invalid resize of render texture. ID: {}, requested size: {} x {}", texture.id, width, height);

        if(width == texture.width && height == texture.height)
            return;

        // Delete current render texture and create a new one
        rlUnloadTexture(texture.color);
        rlUnloadTexture(texture.depth);
        rlUnloadFramebuffer(texture.id);

        texture = RenderTexture{width, height};
    }

    void unloadRenderTextureOpenGL(RenderTexture& texture)
    {
        // We have to unload the textures before unloading the framebuffer
        if(texture.id > 0)
        {
            rlUnloadTexture(texture.color);
            rlUnloadTexture(texture.depth);
            rlUnloadFramebuffer(texture.id);
        }
    }

    // Render texture Ex

    void initRenderTextureExOpenGL(RenderTextureEx& texture, int32 width, int32 height)
    {
        initFramebuffer(texture, width, height);

        // Create red integer texture
        texture.redInt = rlLoadTexture(nullptr, width, height, RL_PIXELFORMAT_UNCOMPRESSED_R32I, 1);

        rlActiveDrawBuffers(2); // Use two color buffers for drawing

        // Attach color, red integer and depth textures to FBO
        rlFramebufferAttach(texture.id, texture.color, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_TEXTURE2D, 0);
        rlFramebufferAttach(texture.id, texture.redInt, RL_ATTACHMENT_COLOR_CHANNEL1, RL_ATTACHMENT_TEXTURE2D, 0);
        rlFramebufferAttach(texture.id, texture.depth, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_TEXTURE2D, 0);

        // Check if FBO is valid
        if(!rlFramebufferComplete(texture.id))
        {
            ENGINE_LOG_ERROR("Render texture framebuffer is incomplete. Color tex ID: {}, Red Int tex ID: {}, Depth tex ID: {}",
                texture.color, texture.redInt, texture.depth);

            rlUnloadTexture(texture.color);
            rlUnloadTexture(texture.redInt);
            rlUnloadTexture(texture.depth);
            rlUnloadFramebuffer(texture.id);
            texture.id = texture.color = texture.redInt = texture.depth = 0;
        }

        rlDisableFramebuffer();
    }

    void resizeRenderTextureExOpenGL(RenderTextureEx& texture, int32 width, int32 height)
    {
        MIRR_ASSERT_CORE_RETURN(texture.id > 0 && width > 0 && height > 0,
            "Invalid resize of render texture. ID: {}, requested size: {} x {}", texture.id, width, height);

        if(width == texture.width && height == texture.height)
            return;

        // Delete current render texture and create a new one
        rlUnloadTexture(texture.color);
        rlUnloadTexture(texture.redInt);
        rlUnloadTexture(texture.depth);
        rlUnloadFramebuffer(texture.id);

        texture = RenderTextureEx{width, height};
    }

    void clearColorAttachmentOpenGL(Attachment index, int32 value)
    {
        glClearBufferiv(GL_COLOR, static_cast<int32>(index), &value); 
    }

    int32 readRenderTexPixelOpenGL(const RenderTextureEx& texture, Attachment index, int32 x, int32 y)
    {
        MIRR_ASSERT_CORE_RETURN_VALUE(texture.id > 0, /*return*/ -1, "Invalid render texture. ID: 0");

		int32 pixelData = -1;

        rlEnableFramebuffer(texture.id);

        glReadBuffer(GL_COLOR_ATTACHMENT0 + static_cast<int32>(index));
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);

        rlDisableFramebuffer();

		return pixelData;
    }

    void unloadRenderTextureExOpenGL(RenderTextureEx& texture)
    {
        // We have to unload the textures before unloading the framebuffer
        if(texture.id > 0)
        {
            rlUnloadTexture(texture.color);
            rlUnloadTexture(texture.redInt);
            rlUnloadTexture(texture.depth);
            rlUnloadFramebuffer(texture.id);
        }
    }
} // namespace mirras
