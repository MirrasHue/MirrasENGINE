#pragma once

#include "Core/Types/Basic.h"
#include "Core/Fwd.h"

#include <glm/detail/type_vec2.hpp>
#include <glm/detail/type_vec4.hpp>
#include <glm/fwd.hpp>

#include <string_view>

namespace mirras
{
    class Renderer
    {
    public:
        enum class Backend : uint8
        {
            OpenGL
        };

    public:
        Renderer() = delete;

        static void init(Backend backend);
        static void shutdown();
        
        static void setWindowViewport(int32 x, int32 y, int32 width, int32 height);
        
        static void clearBackBuffers();
        static void setClearColor(float r, float g, float b, float a = 1.f);
        // Sets a value to be stored in a second color buffer attachment (RED_INTEGER), if present
        static void setPixelOutputData(int32 value);

        // These two are already called for us by the engine, no need to call them ourselves
        static void beginDrawing();
        static void endDrawing();

        static void beginTextureDrawing(const RenderTexture2D& texture);
        static void endTextureDrawing();

        static void beginMode2D(const Camera2D& camera);
        static void endMode2D();

        static void setLineWidth(float width);

        // 2D primitives
        static void drawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color);
        static void drawLine(glm::vec2 start, glm::vec2 end, const glm::vec4& color);

        // Points should be provided in counter clockwise order
        static void drawTriangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec4& color);
        static void drawTriangle(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, const glm::vec4& color);

        static void drawRectangle(const glm::vec3& topLeft, glm::vec2 size, glm::vec2 localOrigin, const glm::vec4& color, float rotation = 0.f);
        static void drawRectangle(glm::vec2 topLeft, glm::vec2 size, glm::vec2 localOrigin, const glm::vec4& color, float rotation = 0.f);

        static void drawCircle(const glm::vec3& center, float radius, const glm::vec4& color, int32 segments = 36);
        static void drawCircle(glm::vec2 center, float radius, const glm::vec4& color, int32 segments = 36);

        // Draws a circle with controllable fill and fade factors, without relying on segments. Limited to flat colors,
        // because internally, a dedicated shader is used to draw the circle, so it is not affected by external shaders
        static void drawShaderCircle(const glm::vec3& center, float radius, const glm::vec4& color, float fillFactor = 1.f, float fadeFactor = 0.007f);
        static void drawShaderCircle(glm::vec2 center, float radius, const glm::vec4& color, float fillFactor = 1.f, float fadeFactor = 0.007f);
        // End 2D primitives

        // Pass {} as the second parameter if you want to use the whole texture, not just part of it
        static void drawTexture(const Texture& texture, const rect4f& texSampleArea, const glm::vec3& targetTopLeft, glm::vec2 targetSize = {},
                                glm::vec2 targetOrigin = {}, float rotation = 0.f, const glm::vec4& tintColor = glm::vec4{1.f});
        
        // Pass {} as the second parameter if you want to use the whole texture
        static void drawTexture(const Texture& texture, const rect4f& texSampleArea, glm::vec2 targetTopLeft, glm::vec2 targetSize = {},
                                glm::vec2 targetOrigin = {}, float rotation = 0.f, const glm::vec4& tintColor = glm::vec4{1.f});

        static void drawText(std::wstring_view text, const Font& font, const glm::vec3& topLeft, float fontSize = 40.f,
                             const glm::vec4& color = glm::vec4{1.f}, float kerning = 0.f, float lineSpacing = 0.f);

        static void drawText(std::wstring_view text, const Font& font, glm::vec2 topLeft, float fontSize = 40.f,
                            const glm::vec4& color = glm::vec4{1.f}, float kerning = 0.f, float lineSpacing = 0.f);

        static Backend getBackend() { return currentBackend; }
    
    private:
        inline static Backend currentBackend = Backend::OpenGL;

    private:
        // In the eventual support of another graphics API, we'd have an abstract class 'BackendRenderer' that declares all
        // the methods we find in OpenGLRenderer. And all renderers would inherit from that class and conform to its interface.
        // We would also need to abstract context creation, given that the default one provided by GLFW is meant for OpenGL
        static class OpenGLRenderer renderer;
        // This would become something like 'static single_ref<BackendRenderer> renderer', and in the 'init' method
        // we would switch on the backend selected by the client and instantiate an actual renderer based on that
    };
} // namespace mirras
