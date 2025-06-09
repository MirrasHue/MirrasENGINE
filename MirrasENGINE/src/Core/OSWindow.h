#pragma once

#include "Core/Types/Basic.h"
#include "Core/Fwd.h"

#include <string_view>
#include <functional>

struct GLFWwindow;

namespace mirras
{
    struct WindowSpecs
    {
        std::string_view title;
        std::string_view iconFilepath;
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

        void init(const WindowSpecs& windowSpecs);

        void setOnEventCallback(const EventCallback& callback) { appCallbacks.onEvent = callback; }

        void makeContextCurrent(bool makeCurrent) const;

        void makeVisible(bool visible) const;

        vec2i getFramebufferSize() const;
        static vec2i getInitialFbSize() { return OSWindow::initialFbSize; }

        bool shouldClose() const;

        void swapBuffers() const;

        void waitEvents() const;
        void pollEvents() const;

        static void setVSync(bool VSync);
        static bool isVSynced() { return OSWindow::VSyncEnabled; }

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

        // There's only one window per application lifecycle, otherwise these wouldn't be static
        inline static vec2i initialFbSize; // Needed by Camera2D
        inline static bool VSyncEnabled = false;
    };
} // namespace mirras
