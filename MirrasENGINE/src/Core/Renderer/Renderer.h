#pragma once

#include "Core/BasicTypes.h"
#include <glm/fwd.hpp>

namespace mirras
{
    class Camera2D;

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
        
        static void setViewport(int32 x, int32 y, int32 width, int32 height);
        
        static void clearBackBuffers();
        static void setClearColor(float r, float g, float b, float a = 1.f);

        static void beginDrawing();
        static void endDrawing();

        static void beginMode2D(const Camera2D& camera);
        static void endMode2D();

        // 2D shapes
        static void drawTriangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec4& color);
        static void drawTriangle(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, const glm::vec4& color);

        static void drawRectangle(const glm::vec3& topLeft, glm::vec2 size, glm::vec2 localOrigin, const glm::vec4& color, float rotation = 0.f);
        static void drawRectangle(glm::vec2 topLeft, glm::vec2 size, glm::vec2 localOrigin, const glm::vec4& color, float rotation = 0.f);

        static Backend getBackend() { return currentBackend; }
    
    private:
        inline static Backend currentBackend = Backend::OpenGL;

    private:
        // In the eventual support of another graphics API, we'd have an abstract class 'BackendRenderer' that declares all
        // the methods we find in OpenGLRenderer. And all renderers would inherit from that class and conform to its interface.
        // We would also need to abstract context creation, given that the default one provided by GLFW is meant for OpenGL
        static class OpenGLRenderer renderer;
        // This would become something like 'static single_ref<class BackendRenderer> renderer', and in the 'init'
        // method we'd switch on the backend selected by the client and instantiate an actual renderer based on that
    };
}