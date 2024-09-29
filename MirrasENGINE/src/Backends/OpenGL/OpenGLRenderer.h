#pragma once

#include "Core/BasicTypes.h"

#include <glm/fwd.hpp>

namespace mirras
{
    class Camera2D;
    class Texture;

    class OpenGLRenderer
    {
    public:
        void init();
        void shutdown();

        void setViewport(int32 x, int32 y, int32 width, int32 height);
        
        void clearBackBuffers();
        void setClearColor(float r, float g, float b, float a = 1.f);

        void beginDrawing();
        void endDrawing();

        void beginMode2D(const Camera2D& camera);
        void endMode2D();

        void drawTriangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec4& color);
        void drawTriangle(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, const glm::vec4& color);

        void drawRectangle(const glm::vec3& topLeftPos, glm::vec2 size, glm::vec2 localOrigin, const glm::vec4& color, float rotation);
        void drawRectangle(glm::vec2 topLeftPos, glm::vec2 size, glm::vec2 localOrigin, const glm::vec4& color, float rotation);

        void drawTexture(const Texture& texture, rect4i texSampleArea, const glm::vec3& targetTopLeft, glm::vec2 targetSize,
                         glm::vec2 targetOrigin, float rotation, const glm::vec4& tintColor);
        
        void drawTexture(const Texture& texture, rect4i texSampleArea, glm::vec2 targetTopLeft, glm::vec2 targetSize,
                         glm::vec2 targetOrigin, float rotation, const glm::vec4& tintColor);
    };
}