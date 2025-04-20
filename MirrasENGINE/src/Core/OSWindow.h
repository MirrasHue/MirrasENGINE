#pragma once

#include "Core/Types/Basic.h"
#include "Core/Fwd.h"

// So that it doesn't include GL/gl.h (which defines __gl_h_, making Glad complain)
#define GLFW_INCLUDE_NONE
#include <glfw/glfw3.h>

#include <string_view>
#include <functional>

namespace mirras
{
    struct WindowSpecs
    {
        std::string_view title;
        int32 width  = 1280;
        int32 height = 720;
        int32 minWidth  = -1; // Both need to be set in
        int32 minHeight = -1; // order to take effect
        bool fullScreen = false;
        bool VSync = true;
        bool keepAspectRatio = false;
    };

    class OSWindow
    {
    public:
        using EventCallback = std::function<void(Event&)>;

        OSWindow(const WindowSpecs& windowSpecs);

        void setOnEventCallback(const EventCallback& callback) { appCallbacks.onEvent = callback; }

        void makeContextCurrent(bool makeCurrent) const { makeCurrent ? glfwMakeContextCurrent(windowHandle) : glfwMakeContextCurrent(nullptr); }

        vec2i getFramebufferSize() const;
        vec2i getInitialFbSize() const { return initialFbSize; }

        bool shouldClose() const { return glfwWindowShouldClose(windowHandle); }

        void swapBuffers() const { glfwSwapBuffers(windowHandle); }

        void waitEvents()  const { glfwWaitEvents(); }

        void pollEvents()  const { glfwPollEvents(); }

        static void setVSync(bool VSync) { glfwSwapInterval(VSync); VSyncEnabled = VSync; }
        static bool isVSynced() { return VSyncEnabled; }

        operator GLFWwindow*() const { return windowHandle; }

        ~OSWindow();

    private:
        void setGLFWCallbacks();

    private:
        struct AppCallbacks
        {
            EventCallback onEvent;
        }
        appCallbacks;

        GLFWwindow* windowHandle = nullptr;
        vec2i initialFbSize; // Used by Camera2D

        inline static bool VSyncEnabled = false;
    };
} // namespace mirras
