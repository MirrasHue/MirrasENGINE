#ifndef __APPLE__
    #define GRAPHICS_API_OPENGL_43 // <- Comment this to use only OpenGL 3.3 functionality
#endif

#include "Backends/OpenGL/OpenGLRenderer.h"

#include "Backends/OpenGL/OpenGLShader.h"
#include "Backends/OpenGL/OpenGLTexture.h"
#include "Backends/OpenGL/OpenGLLog.h"

#include "Core/OSWindow.h"
#include "Core/Renderer/Camera2D.h"
#include "Core/Renderer/Font.h"
#include "Core/Types/Basic.h"
#include "Core/Asserts.h"
#include "Core/Utils.h"
#include "Core/Log.h"

#define RLGL_IMPLEMENTATION
#include <raylib/rlgl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

//#define GLM_FORCE_INTRINSICS
#include <glm/gtc/type_ptr.hpp>

#include <msdf/msdf-atlas-gen.h> // For font rendering

#include <numbers>
#include <cmath>

// Using raylib's core as a reference on how to use rlgl.h

namespace mirras
{
    // Setup viewport and internal projection/modelview matrices
    static void resetViewport(int32 x, int32 y, int32 width, int32 height);

    static vec2i windowFbSize;
    static vec2i windowFbInitialSize;
    static vec2i framebufferSize;
    static vec2i currentFbInitialSize;

    void OpenGLRenderer::init()
    {
        OpenGLLog::init();

        if(!gladLoadGL(glfwGetProcAddress))
            external_adversity("Could not initialize GLAD\n");

        rlLoadExtensions();

        auto [width, height] = OSWindow::getInitialFbSize();

        rlglInit(width, height);

        resetViewport(0, 0, width, height);

        windowFbInitialSize = {width , height};
        windowFbSize = windowFbInitialSize;
        currentFbInitialSize = windowFbInitialSize;

        // So that we can use the Z axis to determine the draw order
        // independent of the draw call order (for different Z values)
        rlEnableDepthTest();
    }

    void OpenGLRenderer::shutdown()
    {
        rlglClose();
    }

    void resetViewport(int32 x, int32 y, int32 width, int32 height)
    {
        glViewport(x, y, width, height);

        framebufferSize.x = width;
        framebufferSize.y = height;

        rlMatrixMode(RL_PROJECTION);
        rlLoadIdentity();

        rlOrtho(x, width, height, y, 0.0, 1.0); // Place origin at the top left corner

        rlMatrixMode(RL_MODELVIEW);
        rlLoadIdentity();
    }

    void OpenGLRenderer::setWindowViewport(int32 x, int32 y, int32 width, int32 height)
    {
        resetViewport(x, y, width, height);
        windowFbSize.x = width;
        windowFbSize.y = height;
    }

    void OpenGLRenderer::clearBackBuffers()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenGLRenderer::setClearColor(float r, float g, float b, float a)
    {
        glClearColor(r, g, b, a);
    }

    void OpenGLRenderer::setPixelOutputData(int32 value)
    {
        rlPixelOutputData(value);
    }

    void OpenGLRenderer::beginDrawing()
    {
        rlLoadIdentity();
    }

    void OpenGLRenderer::endDrawing()
    {
        rlDrawRenderBatchActive();
    }

    void OpenGLRenderer::beginTextureDrawing(const RenderTexture2D& texture)
    {
        MIRR_ASSERT_CORE_RETURN(texture.id > 0 && texture.color,
            "Invalid render texture. ID: {}, null color: {}", texture.id, !(bool)texture.color);

        rlDrawRenderBatchActive();
        rlEnableFramebuffer(texture.id);

        resetViewport(0, 0, texture.width, texture.height);

        currentFbInitialSize = texture.initialSize;
    }

    void OpenGLRenderer::endTextureDrawing()
    {
        rlDrawRenderBatchActive();

        rlDisableFramebuffer();

        resetViewport(0, 0, windowFbSize.x, windowFbSize.y);

        currentFbInitialSize = windowFbInitialSize;
    }

    void OpenGLRenderer::beginMode2D(const Camera2D& camera)
    {
        rlDrawRenderBatchActive();
        rlLoadIdentity();
        rlMultMatrixf(glm::value_ptr(camera.getViewMatrix(framebufferSize, currentFbInitialSize)));
    }

    void OpenGLRenderer::endMode2D()
    {
        rlDrawRenderBatchActive();
        rlLoadIdentity();
    }

    void OpenGLRenderer::setLineWidth(float width)
    {
        glLineWidth(width);
    }

    void OpenGLRenderer::drawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color)
    {
        rlBegin(RL_LINES);
            rlColor4f(color.r, color.g, color.b, color.a);

            rlVertex3f(start.x, start.y, start.z);
            rlVertex3f(end.x, end.y, end.z);
        rlEnd();
    }

    void OpenGLRenderer::drawLine(glm::vec2 start, glm::vec2 end, const glm::vec4& color)
    {
        drawLine(glm::vec3{start, rlGetCurrentDrawDepth()}, glm::vec3{end, rlGetCurrentDrawDepth()}, color);
    }

    void OpenGLRenderer::drawTriangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec4& color)
    {
        rlBegin(RL_TRIANGLES);
            rlColor4f(color.r, color.g, color.b, color.a);

            rlVertex3f(p1.x, p1.y, p1.z);
            rlVertex3f(p2.x, p2.y, p2.z);
            rlVertex3f(p3.x, p3.y, p3.z);
        rlEnd();
    }

    void OpenGLRenderer::drawTriangle(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, const glm::vec4& color)
    {
        float drawDepth = rlGetCurrentDrawDepth();
        drawTriangle(glm::vec3{p1, drawDepth}, glm::vec3{p2, drawDepth}, glm::vec3{p3, drawDepth}, color);
    }

    static std::array<glm::vec2, 4> calculateVertexPositions(const rect4f& rectangle, glm::vec2 localOrigin, float rotation)
    {
        const float x = rectangle.x;
        const float y = rectangle.y;
        const float width = rectangle.width;
        const float height = rectangle.height;

        glm::vec2 topLeft{x, y};
        glm::vec2 topRight{x + width, y};
        glm::vec2 bottomLeft{x, y + height};
        glm::vec2 bottomRight{x + width, y + height};

        if(rotation)
        {
            const float cosTheta = std::cos(glm::radians(rotation));
            const float sinTheta = std::sin(glm::radians(rotation));

            const float dx = -localOrigin.x;
            const float dy = -localOrigin.y;

            // Translate local origin to the global origin, apply rotation, and translate it back

            topLeft.x = dx * cosTheta - dy * sinTheta - dx + x;
            topLeft.y = dx * sinTheta + dy * cosTheta - dy + y;

            topRight.x = (dx + width) * cosTheta - dy * sinTheta - dx + x;
            topRight.y = (dx + width) * sinTheta + dy * cosTheta - dy + y;

            bottomLeft.x = dx * cosTheta - (dy + height) * sinTheta - dx + x;
            bottomLeft.y = dx * sinTheta + (dy + height) * cosTheta - dy + y;

            bottomRight.x = (dx + width) * cosTheta - (dy + height) * sinTheta - dx + x;
            bottomRight.y = (dx + width) * sinTheta + (dy + height) * cosTheta - dy + y;
        }

        return std::array<glm::vec2, 4>{topLeft, topRight, bottomLeft, bottomRight};
    }

    void OpenGLRenderer::drawRectangle(const glm::vec3& topLeftPos, glm::vec2 size, glm::vec2 localOrigin, const glm::vec4& color, float rotation)
    {
        auto[topLeft,    topRight,
             bottomLeft, bottomRight] = calculateVertexPositions(rect4f{topLeftPos.x, topLeftPos.y, size.x, size.y}, localOrigin, rotation);

        const float z = topLeftPos.z;

        rlBegin(RL_TRIANGLES);
            rlColor4f(color.r, color.g, color.b, color.a);

            rlVertex3f(topLeft.x, topLeft.y, z);
            rlVertex3f(bottomLeft.x, bottomLeft.y, z);
            rlVertex3f(topRight.x, topRight.y, z);

            rlVertex3f(topRight.x, topRight.y, z);
            rlVertex3f(bottomLeft.x, bottomLeft.y, z);
            rlVertex3f(bottomRight.x, bottomRight.y, z);
        rlEnd();
    }

    void OpenGLRenderer::drawRectangle(glm::vec2 topLeftPos, glm::vec2 size, glm::vec2 localOrigin, const glm::vec4& color, float rotation)
    {
        drawRectangle(glm::vec3{topLeftPos, rlGetCurrentDrawDepth()}, size, localOrigin, color, rotation);
    }

    void OpenGLRenderer::drawCircle(const glm::vec3& center, float radius, const glm::vec4& color, int32 segments)
    {
        if(segments < 3)
        {
            ENGINE_LOG_WARN("Trying to draw circle with less than 3 segments");
            return;
        }

        float angleStep = 2.f * std::numbers::pi / segments; // In radians

        rlBegin(RL_TRIANGLES);
            rlColor4f(color.r, color.g, color.b, color.a);

            for(int i = 0; i < segments; ++i)
            {
                float angle = angleStep * i;

                rlVertex3f(center.x, center.y, center.z);
                rlVertex3f(center.x + cosf(angle + angleStep) * radius, center.y + sinf(angle + angleStep) * radius, center.z);
                rlVertex3f(center.x + cosf(angle) * radius, center.y + sinf(angle) * radius, center.z);
            }
        rlEnd();
    }

    void OpenGLRenderer::drawCircle(glm::vec2 center, float radius, const glm::vec4& color, int32 segments)
    {
        drawCircle(glm::vec3{center, rlGetCurrentDrawDepth()}, radius, color, segments);
    }

    void OpenGLRenderer::drawShaderCircle(const glm::vec3& center, float radius, const glm::vec4& color, float fillFactor, float fadeFactor)
    {
        if(fillFactor < 0.f || fadeFactor < 0.f)
        {
            ENGINE_LOG_WARN("Fill or fade factors can't be negative");
            return;
        }

        // Embed fragment source here, it seemed overkill to load it from a file in disk (it would also complicate things
        // because it depends on the working directory, which is defined by the client, or where the executable is located)
        // TODO: use #embed when it gets widely supported
        static const std::string_view circleFragSrc = R"(
            #version 330 core

            in vec2 fragTexCoord;
            in vec4 fragColor;
            flat in int fragOutputData;

            out vec4 finalColor;
            out int pixelOutputData;

            uniform float fillFactor;
            uniform float fadeFactor;

            void main()
            {
                // Adapted from Cherno's circle shader
                float distance = 1.f - length(fragTexCoord);
                float alpha = smoothstep(0.f, fadeFactor, distance);
                alpha *= smoothstep(fillFactor + fadeFactor + 0.0001f, fillFactor, distance);

                if(alpha == 0.f)
                    discard;

                finalColor = fragColor;
                finalColor.a *= alpha;
                pixelOutputData = fragOutputData;
            }
        )";
        
        static OpenGLShader circleShader{{}, circleFragSrc};
        
        static int32 fillFactorLoc = circleShader.getUniformLocation("fillFactor");
        static int32 fadeFactorLoc = circleShader.getUniformLocation("fadeFactor");
        
        // Save the current shader and restore it at the end, just in case
        // this function is being called while another custom shader is active
        uint32 priorShaderId = rlGetCurrentShaderId();
        int32* priorShaderLocs = rlGetCurrentShaderLocs();

        circleShader.setFloat(fillFactorLoc, fillFactor);
        circleShader.setFloat(fadeFactorLoc, fadeFactor);
        circleShader.makeActive();

        rlBegin(RL_QUADS);
            rlColor4f(color.r, color.g, color.b, color.a);

            rlTexCoord2f(-1.f, -1.f); // Top left
            rlVertex3f(center.x - radius, center.y - radius, center.z);

            rlTexCoord2f(-1.f, 1.f); // Bottom left
            rlVertex3f(center.x - radius, center.y + radius, center.z);

            rlTexCoord2f(1.f, 1.f); // Bottom right
            rlVertex3f(center.x + radius, center.y + radius, center.z);

            rlTexCoord2f(1.f, -1.f); // Top right
            rlVertex3f(center.x + radius, center.y - radius, center.z);
        rlEnd();

        rlSetShader(priorShaderId, priorShaderLocs);
    }

    void OpenGLRenderer::drawShaderCircle(glm::vec2 center, float radius, const glm::vec4& color, float fillFactor, float fadeFactor)
    {
        drawShaderCircle(glm::vec3{center, rlGetCurrentDrawDepth()}, radius, color, fillFactor, fadeFactor);
    }

    void OpenGLRenderer::drawTexture(const Texture& texture, const rect4f& texSampleArea, const glm::vec3& targetTopLeft, glm::vec2 targetSize, glm::vec2 targetOrigin, float rotation, const glm::vec4& tintColor)
    {
        if(texture.id == 0)
        {
            ENGINE_LOG_WARN("Not possible to draw texture with ID: 0");
            return;
        }

        rect4f sampleArea = texSampleArea;
        const float texWidth = texture.width;
        const float texHeight = texture.height;

        if(sampleArea == rect4f{}) // Make use of the entire texture
        {
            sampleArea.width = texWidth;
            sampleArea.height = texHeight;
        }

        if(targetSize == glm::vec2{}) // Use the texture's original size for drawing
        {
            targetSize.x = texWidth;
            targetSize.y = texHeight;
        }

        auto[topLeft,    topRight,
             bottomLeft, bottomRight] = calculateVertexPositions(rect4f{targetTopLeft.x, targetTopLeft.y, targetSize.x, targetSize.y}, targetOrigin, rotation);

        const float z = targetTopLeft.z;

        rlSetTexture(texture.id);

        rlBegin(RL_QUADS);
            rlColor4f(tintColor.r, tintColor.g, tintColor.b, tintColor.a);

            rlTexCoord2f(sampleArea.x / texWidth, sampleArea.y / texHeight);
            rlVertex3f(topLeft.x, topLeft.y, z);

            rlTexCoord2f(sampleArea.x / texWidth, (sampleArea.y + sampleArea.height) / texHeight);
            rlVertex3f(bottomLeft.x, bottomLeft.y, z);

            rlTexCoord2f((sampleArea.x + sampleArea.width) / texWidth, (sampleArea.y + sampleArea.height) / texHeight);
            rlVertex3f(bottomRight.x, bottomRight.y, z);

            rlTexCoord2f((sampleArea.x + sampleArea.width) / texWidth, sampleArea.y / texHeight);
            rlVertex3f(topRight.x, topRight.y, z);
        rlEnd();

        rlSetTexture(0);
    }

    void OpenGLRenderer::drawTexture(const Texture& texture, const rect4f& texSampleArea, glm::vec2 targetTopLeft, glm::vec2 targetSize,glm::vec2 targetOrigin, float rotation, const glm::vec4& tintColor)
    {
        drawTexture(texture, texSampleArea, glm::vec3{targetTopLeft, rlGetCurrentDrawDepth()}, targetSize, targetOrigin, rotation, tintColor);
    }

    void OpenGLRenderer::drawText(std::wstring_view text, const Font& font, const glm::vec3& topLeftPos, float fontSize, const glm::vec4& color, float kerning, float lineSpacing)
    {
        if(!font.geometry || !font.atlasTexture)
        {
            ENGINE_LOG_ERROR("Font was not properly initialized, unable to render text with it");
            return;
        }

        static const std::string_view msdfFragSrc = R"(
            #version 330 core

            in vec2 fragTexCoord;
            in vec4 fragColor;
            flat in int fragOutputData;

            out vec4 finalColor;
            out int pixelOutputData;

            uniform sampler2D msdfFontAtlas;

            float screenPxRange()
            {
                const float pxRange = 2.0; // Set to the same pixel range value used in TightAtlasPacker
                vec2 unitRange = vec2(pxRange) / vec2(textureSize(msdfFontAtlas, 0));
                vec2 screenTexSize = vec2(1.0) / fwidth(fragTexCoord);
                return max(0.5 * dot(unitRange, screenTexSize), 1.0);
            }

            float median(float r, float g, float b)
            {
                return max(min(r, g), min(max(r, g), b));
            }

            void main()
            {
                vec3 msd = texture(msdfFontAtlas, fragTexCoord).rgb;
                float sd = median(msd.r, msd.g, msd.b);
                float screenPxDistance = screenPxRange() * (sd - 0.5);
                float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
                
                vec4 bgColor = vec4(0.0);

                finalColor = mix(bgColor, fragColor, opacity);
                pixelOutputData = fragOutputData;
            }
        )";
        
        static OpenGLShader msdfTextShader{{}, msdfFragSrc};

        const auto& fontGeometry = *font.geometry;
        const auto& metrics = fontGeometry.getMetrics();
        const auto& fontAtlas = *font.atlasTexture;

        const float fontSizeScale = 1.f / (metrics.ascenderY - metrics.descenderY);
        const float scaleFactor = fontSize / fontSizeScale;
        
        const float emptySpaceAdvance = fontGeometry.getGlyph(' ')->getAdvance();

        float x = 0.f;
        float y = fontSizeScale * metrics.ascenderY;

        const float atlasWidth = fontAtlas.width;
        const float atlasHeight = fontAtlas.height;
        const float drawDepth = topLeftPos.z;

        msdfTextShader.makeActive();
        rlSetTexture(fontAtlas.id);
		
        for(size_t i = 0; i < text.size(); ++i)
        {
            wchar_t character = text[i];

            if(character == '\r')
                continue;

            if(character == '\n')
            {
                x = 0.f;
                y += fontSizeScale * metrics.lineHeight + lineSpacing;
                continue;
            }

            if(character == '\t') // Using 4 spaces for tab character
            {
                x += 4.f * (fontSizeScale * emptySpaceAdvance + kerning);
                continue;
            }

            auto glyph = fontGeometry.getGlyph(character);
            
            if(!glyph)
                glyph = fontGeometry.getGlyph('?');

            if(!glyph)
            {
                ENGINE_LOG_WARN("Limited font charset was loaded, some basic characters are missing. Unable to draw the requested text");
                break;
            }

            if(!glyph->isWhitespace())
            {
                double left{}, bottom{}, right{}, top{};

                glyph->getQuadAtlasBounds(left, bottom, right, top);
                rect4f texSampleArea(left, bottom, right - left, top - bottom);

                glyph->getQuadPlaneBounds(left, bottom, right, top);
                glm::vec4 glyphQuad{left, -top, right - left, top - bottom};
                
                glyphQuad *= fontSizeScale;
                glyphQuad += glm::vec4(x, y, 0.f, 0.f);

                glyphQuad *= scaleFactor;

                glyphQuad += glm::vec4(topLeftPos.x, topLeftPos.y, 0.f, 0.f);

                auto[topLeft,    topRight,
                     bottomLeft, bottomRight] = calculateVertexPositions((const rect4f&)glyphQuad, {}, 0.f);                

                rlBegin(RL_QUADS);
                    rlColor4f(color.r, color.g, color.b, color.a);

                    rlTexCoord2f(texSampleArea.x / atlasWidth, texSampleArea.y / atlasHeight);
                    rlVertex3f(topLeft.x, topLeft.y, drawDepth);

                    rlTexCoord2f(texSampleArea.x / atlasWidth, (texSampleArea.y + texSampleArea.height) / atlasHeight);
                    rlVertex3f(bottomLeft.x, bottomLeft.y, drawDepth);

                    rlTexCoord2f((texSampleArea.x + texSampleArea.width) / atlasWidth, (texSampleArea.y + texSampleArea.height) / atlasHeight);
                    rlVertex3f(bottomRight.x, bottomRight.y, drawDepth);

                    rlTexCoord2f((texSampleArea.x + texSampleArea.width) / atlasWidth, texSampleArea.y / atlasHeight);
                    rlVertex3f(topRight.x, topRight.y, drawDepth);
                rlEnd();
            }

            if(i < text.size() - 1)
            {
                double advance = glyph->getAdvance();
                wchar_t nextCharacter = text[i + 1];

                fontGeometry.getAdvance(advance, character, nextCharacter);

                x += fontSizeScale * advance + kerning;
            }
        }

        rlSetTexture(0);
        msdfTextShader.makeInactive();
    }

    void OpenGLRenderer::drawText(std::wstring_view text, const Font& font, glm::vec2 topLeftPos, float fontSize, const glm::vec4& color, float kerning, float lineSpacing)
    {
        drawText(text, font, glm::vec3{topLeftPos, rlGetCurrentDrawDepth()}, fontSize, color, kerning, lineSpacing);
    }
} // namespace mirras
