#pragma once

#include "Core/BasicTypes.h"
#include "Core/Fwd.h"

#include <glm/fwd.hpp>

namespace mirras
{
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

        void setLineWidth(float width);

        void drawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color);
        void drawLine(glm::vec2 start, glm::vec2 end, const glm::vec4& color);

        void drawTriangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec4& color);
        void drawTriangle(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, const glm::vec4& color);

        void drawRectangle(const glm::vec3& topLeftPos, glm::vec2 size, glm::vec2 localOrigin, const glm::vec4& color, float rotation);
        void drawRectangle(glm::vec2 topLeftPos, glm::vec2 size, glm::vec2 localOrigin, const glm::vec4& color, float rotation);

        void drawCircle(const glm::vec3& center, float radius, const glm::vec4& color, int32 segments);
        void drawCircle(glm::vec2 center, float radius, const glm::vec4& color, int32 segments);

        void drawShaderCircle(const glm::vec3& center, float radius, const glm::vec4& color, float fillFactor, float fadeFactor);
        void drawShaderCircle(glm::vec2 center, float radius, const glm::vec4& color, float fillFactor, float fadeFactor);

        void drawTexture(const Texture& texture, rect4f texSampleArea, const glm::vec3& targetTopLeft, glm::vec2 targetSize,
                         glm::vec2 targetOrigin, float rotation, const glm::vec4& tintColor);
        
        void drawTexture(const Texture& texture, rect4f texSampleArea, glm::vec2 targetTopLeft, glm::vec2 targetSize,
                         glm::vec2 targetOrigin, float rotation, const glm::vec4& tintColor);

        void drawText(std::wstring_view text, const Font& font, const glm::vec3& topLeftPos, float fontSize, const glm::vec4& color, float kerning, float lineSpacing);
        void drawText(std::wstring_view text, const Font& font, glm::vec2 topLeftPos, float fontSize, const glm::vec4& color, float kerning, float lineSpacing);
    };
} // namespace mirras
