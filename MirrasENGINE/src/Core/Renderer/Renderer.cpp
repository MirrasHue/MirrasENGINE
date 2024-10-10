#include "Core/Renderer/Renderer.h"

#include "Backends/OpenGL/OpenGLRenderer.h"

#include <glm/vec2.hpp>

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

    void Renderer::setLineWidth(float width)
    {
        renderer.setLineWidth(width);
    }

    void Renderer::drawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color)
    {
        renderer.drawLine(start, end, color);
    }

    void Renderer::drawLine(glm::vec2 start, glm::vec2 end, const glm::vec4& color)
    {
        renderer.drawLine(start, end, color);
    }

    void Renderer::drawTriangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec4& color)
    {
        renderer.drawTriangle(p1, p2, p3, color);
    }

    void Renderer::drawTriangle(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, const glm::vec4& color)
    {
        renderer.drawTriangle(p1, p2, p3, color);
    }

    void Renderer::drawRectangle(const glm::vec3& topLeft, glm::vec2 size, glm::vec2 localOrigin, const glm::vec4& color, float rotation)
    {
        renderer.drawRectangle(topLeft, size, localOrigin, color, rotation);
    }

    void Renderer::drawRectangle(glm::vec2 topLeft, glm::vec2 size, glm::vec2 localOrigin, const glm::vec4& color, float rotation)
    {
        renderer.drawRectangle(topLeft, size, localOrigin, color, rotation);
    }

    void Renderer::drawCircle(const glm::vec3& center, float radius, const glm::vec4& color, int32 segments)
    {
        renderer.drawCircle(center, radius, color, segments);
    }

    void Renderer::drawCircle(glm::vec2 center, float radius, const glm::vec4& color, int32 segments)
    {
        renderer.drawCircle(center, radius, color, segments);
    }

    void Renderer::drawShaderCircle(const glm::vec3& center, float radius, const glm::vec4& color, float fillFactor, float fadeFactor)
    {
        renderer.drawShaderCircle(center, radius, color, fillFactor, fadeFactor);
    }

    void Renderer::drawShaderCircle(glm::vec2 center, float radius, const glm::vec4& color, float fillFactor, float fadeFactor)
    {
        renderer.drawShaderCircle(center, radius, color, fillFactor, fadeFactor);
    }

    void Renderer::drawTexture(const Texture& texture, rect4i texSampleArea, const glm::vec3& targetTopLeft, glm::vec2 targetSize,glm::vec2 targetOrigin, float rotation, const glm::vec4& tintColor)
    {
        renderer.drawTexture(texture, texSampleArea, targetTopLeft, targetSize, targetOrigin, rotation, tintColor);
    }

    void Renderer::drawTexture(const Texture& texture, rect4i texSampleArea, glm::vec2 targetTopLeft, glm::vec2 targetSize,glm::vec2 targetOrigin, float rotation, const glm::vec4& tintColor)
    {
        renderer.drawTexture(texture, texSampleArea, targetTopLeft, targetSize, targetOrigin, rotation, tintColor);
    }
} // namespace mirras
