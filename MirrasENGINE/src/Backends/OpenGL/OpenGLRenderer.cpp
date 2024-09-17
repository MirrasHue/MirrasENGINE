
#define GRAPHICS_API_OPENGL_43 // <- Comment this to use only OpenGL 3.3 functionality

#include "Backends/OpenGL/OpenGLRenderer.h"

#include "Backends/OpenGL/OpenGLLog.h"

#include "Core/Application.h"
#include "Core/Renderer/Camera2D.h"
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
        glClear(GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT);
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

    void OpenGLRenderer::drawTriangle()
    {
        // Temp
        rlBegin(RL_TRIANGLES);
            rlColor4ub(255, 255, 255, 255);
            rlVertex3f(200.f, 400.f, 0.f);
            rlVertex3f(600.f, 400.f, 0.f);
            rlVertex3f(400.f, 100.f, 0.f);
        rlEnd();
    }
}