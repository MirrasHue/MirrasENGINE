
#define GRAPHICS_API_OPENGL_43 // <- Comment this to use only OpenGL 3.3 functionality

#include "Backends/OpenGL/OpenGLRenderer.h"

#include "Backends/OpenGL/OpenGLLog.h"

#include "Core/Application.h"
#include "Core/Renderer/Camera2D.h"
#include "Core/Renderer/Texture.h"
#include "Core/Utils.h"
#include "Core/BasicTypes.h"

#define RLGL_IMPLEMENTATION
#include <raylib/rlgl.h>

#define GLFW_INCLUDE_NONE
#include <glfw/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
//#define GLM_FORCE_INTRINSICS
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Using raylib's core as a reference on how to use rlgl.h

namespace mirras
{
    // Setup viewport and internal projection/modelview matrices
    static void resetViewport(int32 x, int32 y, int32 width, int32 height);

    static int32 windowFbWidth{};
    static int32 windowFbHeight{};

    void OpenGLRenderer::init()
    {
        OpenGLLog::init();

        if(!gladLoadGL(glfwGetProcAddress))
            external_adversity("Could not initialize GLAD\n");
            
        rlLoadExtensions();

        auto [width, height] = App::getOSWindow().getFramebufferSize();
        
        rlglInit(width, height);

        resetViewport(0, 0, width, height);

        windowFbWidth = width;
        windowFbHeight = height;

        // So that we can use the Z axis to determine the draw order
        // independent of the draw call order (for different Z values)
        rlEnableDepthTest();
    }

    void OpenGLRenderer::shutdown()
    {
        rlglClose();
    }

    glm::mat4 getCameraMatrix(const Camera2D& camera)
    {
        return glm::translate(glm::vec3{windowFbWidth/2.f - camera.getOffset().x, windowFbHeight/2.f - camera.getOffset().y, 0.f}) *
               glm::rotate(glm::radians(camera.rotation), glm::vec3{0.f, 0.f, 1.f}) *
               glm::scale(glm::vec3{camera.zoom, camera.zoom, 1.f}) *
               glm::translate(glm::vec3{-camera.position.x, -camera.position.y, 0.f});
    }

    void resetViewport(int32 x, int32 y, int32 width, int32 height)
    {
        glViewport(x, y, width, height);
        rlSetFramebufferWidth(width);
        rlSetFramebufferHeight(height);

        rlMatrixMode(RL_PROJECTION);
        rlLoadIdentity();

        rlOrtho(x, width, height, y, 0.0, 1.0); // Place origin at the top left corner

        rlMatrixMode(RL_MODELVIEW);
        rlLoadIdentity();
    }

    void OpenGLRenderer::setViewport(int32 x, int32 y, int32 width, int32 height)
    {
        resetViewport(x, y, width, height);
        windowFbWidth = width;
        windowFbHeight = height;
    }

    void OpenGLRenderer::clearBackBuffers()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenGLRenderer::setClearColor(float r, float g, float b, float a)
    {
        glClearColor(r, g, b, a);
    }

    void OpenGLRenderer::beginDrawing()
    {
        rlLoadIdentity();
    }

    void OpenGLRenderer::endDrawing()
    {
        rlDrawRenderBatchActive();
    }

    void OpenGLRenderer::beginMode2D(const Camera2D& camera)
    {
        rlDrawRenderBatchActive();
        rlLoadIdentity();
        rlMultMatrixf(glm::value_ptr(getCameraMatrix(camera)));
    }

    void OpenGLRenderer::endMode2D()
    {
        rlDrawRenderBatchActive();
        rlLoadIdentity();
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

    std::array<glm::vec2, 4> calculateVertexPositions(const rect4f& rectangle, glm::vec2 localOrigin, float rotation)
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
            const float cosTheta = std::cosf(glm::radians(rotation));
            const float sinTheta = std::sinf(glm::radians(rotation));

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

    void OpenGLRenderer::drawTexture(const Texture& texture, rect4i texSampleArea, const glm::vec3& targetTopLeft, glm::vec2 targetSize, glm::vec2 targetOrigin, float rotation, const glm::vec4& tintColor)
    {
        if(texture.id == 0)
        {
            ENGINE_LOG_ERROR("Not possible to draw texture with id 0");
            return;
        }

        if(texSampleArea == rect4i{}) // Make use of the entire texture
        {
            texSampleArea.width = texture.width;
            texSampleArea.height = texture.height;
        }

        auto[topLeft,    topRight,
             bottomLeft, bottomRight] = calculateVertexPositions(rect4f{targetTopLeft.x, targetTopLeft.y, targetSize.x, targetSize.y}, targetOrigin, rotation);

        const float z = targetTopLeft.z;

        const float texWidth = texture.width;
        const float texHeight = texture.height;

        rlSetTexture(texture.id);

        rlBegin(RL_QUADS);
            rlColor4f(tintColor.r, tintColor.g, tintColor.b, tintColor.a);
            rlNormal3f(0.f, 0.f, -1.f); // Normal vector pointing towards viewer

            rlTexCoord2f(texSampleArea.x / texWidth, texSampleArea.y / texHeight);
            rlVertex3f(topLeft.x, topLeft.y, z);

            rlTexCoord2f(texSampleArea.x / texWidth, (texSampleArea.y + texSampleArea.height) / texHeight);
            rlVertex3f(bottomLeft.x, bottomLeft.y, z);

            rlTexCoord2f((texSampleArea.x + texSampleArea.width) / texWidth, (texSampleArea.y + texSampleArea.height) / texHeight);
            rlVertex3f(bottomRight.x, bottomRight.y, z);

            rlTexCoord2f((texSampleArea.x + texSampleArea.width) / texWidth, texSampleArea.y / texHeight);
            rlVertex3f(topRight.x, topRight.y, z);
        rlEnd();

        rlSetTexture(0);
    }

    void OpenGLRenderer::drawTexture(const Texture& texture, rect4i texSampleArea, glm::vec2 targetTopLeft, glm::vec2 targetSize,glm::vec2 targetOrigin, float rotation, const glm::vec4& tintColor)
    {
        drawTexture(texture, texSampleArea, glm::vec3{targetTopLeft, rlGetCurrentDrawDepth()}, targetSize, targetOrigin, rotation, tintColor);
    }
} // namespace mirras
