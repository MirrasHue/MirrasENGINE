#include "Core/Renderer/Renderer.h"

#include "Backends/OpenGL/OpenGLRenderer.h"

namespace mirras
{
    OpenGLRenderer Renderer::renderer;

    void Renderer::init(Backend backend)
    {
        renderer.init();
        currentBackend = backend;
    }

    void Renderer::shutdown()
    {
        renderer.shutdown();
    }

    void Renderer::setViewport(int32 x, int32 y, int32 width, int32 height)
    {
        renderer.setViewport(x, y, width, height);
    }

    void Renderer::clearBackBuffers()
    {
        renderer.clearBackBuffers();
    }

    void Renderer::setClearColor(float r, float g, float b, float a)
    {
        renderer.setClearColor(r, g, b, a);
    }

    void Renderer::beginDrawing()
    {
        renderer.beginDrawing();
    }

    void Renderer::endDrawing()
    {
        renderer.endDrawing();
    }

    void Renderer::beginMode2D(const Camera2D& camera)
    {
        renderer.beginMode2D(camera);
    }

    void Renderer::endMode2D()
    {
        renderer.endMode2D();
    }

    void Renderer::drawTriangle()
    {
        renderer.drawTriangle();
    }
}