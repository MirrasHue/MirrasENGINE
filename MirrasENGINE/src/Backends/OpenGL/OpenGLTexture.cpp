#include "Backends/OpenGL/OpenGLTexture.h"

#include "Core/Log.h"
#include "Core/Asserts.h"

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

    static void applyTextureFilter(uint32 id, TextureFilter filter)
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
        auto* image = stbi_load(imageFilepath.string().c_str(), &width, &height, &channels, 0);

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

        applyTextureFilter(id, filter);
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

        applyTextureFilter(id, specs.filter);
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

    void initRenderTextureOpenGL(RenderTexture2D& texture, int32 width, int32 height)
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
        texture.color = instantiate<OpenGLTexture>();
        
        texture.color->id = rlLoadTexture(nullptr, width, height, RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);

        if(texture.color->id == 0)
        {
            ENGINE_LOG_ERROR("Unable to create color texture for the render texture");

            texture.color.reset();
            rlUnloadFramebuffer(texture.id);
            texture.id = 0;

            return;
        }

        texture.color->width = width;
        texture.color->height = height;
        texture.color->channels = 4; //R8G8B8A8
        texture.color->mipmaps = 1;

        // Create depth texture
        texture.depth = instantiate<OpenGLTexture>();

        texture.depth->id = rlLoadTextureDepth(width, height, false);

        if(texture.depth->id == 0)
        {
            ENGINE_LOG_ERROR("Unable to create depth texture for the render texture");

            texture.color.reset();
            texture.depth.reset();
            rlUnloadFramebuffer(texture.id);
            texture.id = 0;

            return;
        }

        texture.depth->width = width;
        texture.depth->height = height;
        texture.depth->mipmaps = 1;

        // Attach color texture and depth texture to FBO
        rlFramebufferAttach(texture.id, texture.color->id, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_TEXTURE2D, 0);
        rlFramebufferAttach(texture.id, texture.depth->id, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_TEXTURE2D, 0);

        // Check if FBO is valid
        if(!rlFramebufferComplete(texture.id))
        {
            ENGINE_LOG_ERROR("Render texture framebuffer is incomplete for some reason. Deleting it...");

            texture.color.reset();
            texture.depth.reset();
            rlUnloadFramebuffer(texture.id);
            texture.id = 0;

            return;
        }

        rlDisableFramebuffer();
    }

    void resizeRenderTextureOpenGL(RenderTexture2D& texture, int32 width, int32 height)
    {
        MIRR_ASSERT_CORE_RETURN(texture.id > 0 && texture.color && width > 0 && height > 0,
            "Invalid resize of render texture. ID: {}, null color: {}, requested size: {} x {}", texture.id, !(bool)texture.color, width, height);

        if(width == texture.color->width && height == texture.color->height)
            return;

        // Delete previous render texture and create a new one
        texture.color.reset();
        texture.depth.reset();
        rlUnloadFramebuffer(texture.id);

        texture = RenderTexture2D{width, height};
    }

    void unloadRenderTextureOpenGL(RenderTexture2D& texture)
    {
        if(texture.id > 0)
        {
            texture.color.reset();
            texture.depth.reset();
            rlUnloadFramebuffer(texture.id);
        }
    }
} // namespace mirras
