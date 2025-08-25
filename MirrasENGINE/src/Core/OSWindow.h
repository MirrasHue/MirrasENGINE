#pragma once

#include "Core/Types/Basic.h"
#include "Core/Fwd.h"

#include <string_view>

struct GLFWwindow;

namespace mirras
{
    class EventHandler;

    struct WindowSpecs
    {
        std::string_view title;
        std::string_view iconFilepath;
        int32 width  = 1280;
        int32 height = 720;
        int32 minWidth  = -1; // Both need to be set in
        int32 minHeight = -1; // order to take effect
        bool maximized  = false;
        bool fullScreen = false;
        bool keepAspectRatio = false;
        bool VSync = true;
    };

    class OSWindow
    {
    public:
        OSWindow() = default;

        void makeContextCurrent(bool makeCurrent) const;

        void makeVisible(bool visible) const;

        vec2i getFramebufferSize() const;
        static vec2i getInitialFbSize() { return OSWindow::initialFbSize; }

        bool shouldClose() const;

        void swapBuffers() const;

        void waitEvents() const;
        void pollEvents() const;
        void postEmptyEvent() const; // Causes waitEvents to return

        static void setVSync(bool VSync);
        static bool isVSynced() { return OSWindow::VSyncEnabled; }

        operator GLFWwindow*() const { return windowHandle; }

        ~OSWindow();

    private:
        void init(const WindowSpecs& windowSpecs);
        void setAppEventHandler(EventHandler* handler) const;
        void setGLFWCallbacks();

    private:
        GLFWwindow* windowHandle = nullptr;

        // There's only one window per application lifecycle, otherwise these wouldn't be static
        inline static vec2i initialFbSize; // Needed by Camera2D
        inline static bool VSyncEnabled = false;

        friend class App;
    };
} // namespace mirras
