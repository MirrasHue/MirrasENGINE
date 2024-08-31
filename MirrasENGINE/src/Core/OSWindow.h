#pragma once

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <string_view>
#include <cstdlib>

#include "Events/Event.h"
#include "Core/Utils.h"

namespace mirras
{
    struct WindowSpecs
    {
        std::string_view title;
        int32 width = 1280;
        int32 height = 720;
        int32 minWidth = -1; // Both need to be set in
        int32 minHeight = -1;// order to take effect
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

        void makeContextCurrent(bool makeCurrent) { makeCurrent ? glfwMakeContextCurrent(windowHandle) : glfwMakeContextCurrent(nullptr); }

        vec2i getFramebufferSize()
        {
            vec2i size;
            glfwGetFramebufferSize(windowHandle, &size.x, &size.y);
            return size;
        }

        bool shouldClose() { return glfwWindowShouldClose(windowHandle); }

        void swapBuffers() { glfwSwapBuffers(windowHandle); }

        void waitEvents()  { glfwWaitEvents(); }

        void pollEvents()  { glfwPollEvents(); }

        operator GLFWwindow*() { return windowHandle; }

        ~OSWindow()
        {
            glfwDestroyWindow(windowHandle);
            std::atexit(glfwTerminate);
        }

    private:
        void setGLFWCallbacks();

    private:
        struct AppCallbacks
        {
            EventCallback onEvent;
        }
        appCallbacks;

        GLFWwindow* windowHandle = nullptr;
    };
}