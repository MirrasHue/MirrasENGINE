#pragma once

#include "Core/BasicTypes.h"

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
        static void setViewport(int32 x, int32 y, int32 width, int32 height);
        
        static void clearBackBuffers();
        static void setClearColor(float r, float g, float b, float a = 1.f);

        static void beginDrawing();
        static void endDrawing();

        static void beginMode2D(Camera2D& camera);
        static void endMode2D();

        static void drawTriangle();

    private:
        // In the eventual support of another graphics API, we'd have an abstract class 'BackendRenderer' that declares all
        // the methods we find in OpenGLRenderer. And all renderers would inherit from that class and conform to its interface.
        // We would also need to abstract context creation, given that the default one provided by GLFW is meant for OpenGL
        static class OpenGLRenderer renderer;
        // This would become something like 'static single_ref<class BackendRenderer> renderer', and in the 'init'
        // method we'd switch on the backend selected by the client and instantiate an actual renderer based on that
    };
}